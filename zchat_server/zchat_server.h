#ifndef ZCHAT_SERVER_H
#define ZCHAT_SERVER_H
#include "czmq.h"
#include "zmqhelpers.h"

#include <vector>
#include <map>

//Asynchronous zchat_server. USE DEALER AND ROUTER

#include "types.h"
#include "utils.h"
#include "zchat_identity.h"
#include "zchat_message.h"

#define HEARTBEAT_LIVENESS 3 // 3-5 is reasonable
#define HEARTBEAT_INTERVAL 1000 // msecs
#define HEARTBEAT_INTERVAL_INIT 1000 // Initial reconnect
#define HEARTBEAT_INTERVAL_MAX 32000 

typedef std::vector<zchat_identity *> zchat_identitylist_t;

typedef std::map<zchat_string_t, zchat_identity *> zchat_clientmap_t;

struct server_state
{
    zctx_t* context;
    zchat_identitylist_t identities;
    zchat_clientmap_t clients;
    
    const char * server_url;
};

server_state* client_state_init(const char * server_url)
{
    srand(time(0));
    server_state * state = new server_state();
    state->context = zctx_new ();
    state->server_url = server_url;
    return state;
}

void client_state_destroy(server_state* state)
{
    zctx_destroy(&state->context);
    
    for(zchat_clientmap_t::iterator 
        it = state->clients.begin();
        it !=  state->clients.end();
        it++)
    {
        zchat_identity_destroy(it->second);
    }
    
    delete state;
}

void send_message(void * socket, zchat_identity* identity, zchat_message * message)
{
    zframe_t * identity_frame = zframe_new(identity->data, identity->size);
    zframe_send (&identity_frame, socket, ZFRAME_REUSE + ZFRAME_MORE);
    
    zchat_string_t serialised;
    message->SerializeToString(&serialised);
    
    zframe_t * message_frame = zframe_new(serialised.c_str(),serialised.size());
    zframe_send (&message_frame, socket, ZFRAME_REUSE); 
    
    zframe_destroy(&message_frame);
    zframe_destroy(&identity_frame);
}

void send_message_to_recipient(  server_state * state,
                                 void * socket,
                                 const zchat_string_t& recipient,
                                 zchat_message * message)
{
    zchat_clientmap_t::iterator it = state->clients.find(recipient);
    if(it == state->clients.end())
    {
        zchat_log("UNKNOWN recipient %s",recipient.c_str());
        return;
    }
    
    zchat_identity* identity = it->second;
    send_message(socket, identity, message);
}

void send_message_to_all(server_state * state,
                         void * socket,
                         zchat_message * message)
{
    for(zchat_clientmap_t::iterator 
        it = state->clients.begin();
        it !=  state->clients.end();
        it++)
    {
        zchat_string_t resipient = it->first;
        send_message_to_recipient(state, socket,resipient , message);
    }
}


void server_worker_process_message_ping(server_state * state,
                                   void * socket,
                                   zchat_message * message)
{
    message->set_type(zchat_message_message_type_PONG);
    send_message_to_recipient(state, socket, message->sender(), message);
}

void server_worker_process_message(server_state * state,
                                   void * socket,
                                   zchat_message * message)
{
    size_t count_receivers = message->receiver_size();
    if(count_receivers == 0)
    {
        send_message_to_all(state, socket,  message);
    }
    
    for(int i = 0; i < count_receivers; i++)
    {
        const zchat_string_t& recipient = message->receiver(i);
        send_message_to_recipient(state, socket, recipient, message);
    }
    
    //    zframe_send (&identity, socket, ZFRAME_REUSE + ZFRAME_MORE);
    //    zframe_send (&content, socket, ZFRAME_REUSE);        
}

static void
add_client_from_message(server_state * state, zframe_t * identity_frame, zchat_message * message)
{
    zchat_byte_t * identity_data = zframe_data(identity_frame);
    size_t identity_size = zframe_size(identity_frame);
    const zchat_string_t& sender = message->sender();
    zchat_clientmap_t::iterator it = state->clients.find(sender);
    if(it != state->clients.end())    
    {
        zchat_identity* old_identity = it->second;
        if(zchat_identity_is_equals(old_identity, identity_data, identity_size) == true)
        {   
            zchat_identity_reset_liveness(old_identity);
            return;
        }
        
        zchat_identity_destroy(old_identity);
    }
    
    zchat_identity* identity =  zchat_identity_new(identity_data, identity_size);
    state->clients[sender] = identity;
}

///////////////////////////////////////////
static void
server_worker (void *args, zctx_t *ctx, void *pipe)
{
    server_state * state = (server_state*) args;
    
    void *worker = zsocket_new (ctx, ZMQ_DEALER);
    zsocket_connect (worker, "inproc://backend");
    
    while (true) {
        // The DEALER socket gives us the reply envelope and message
        zmsg_t *msg = zmsg_recv (worker);
        zframe_t *identity = zmsg_pop (msg);
        assert (identity);
        zframe_t *content = zmsg_pop (msg);
        assert (content);
        zmsg_destroy (&msg);
        
        zchat_message * message = zchat_message_deserialize_from_zframe(content);
        
        add_client_from_message(state, identity, message);
        zclock_sleep (randof (1000) + 1);
        
        switch(message->type())
        {
        case zchat_message_message_type_MESSAGE:
            server_worker_process_message(state, worker, message);
            break; 
        
        case zchat_message_message_type_PING:
            server_worker_process_message_ping(state, worker, message);
            break;
            
        default:
            zchat_log("UNKNOWN MESSAGE TYPE %d", message->type());
        }
        
        zchat_message_destroy(message);
        zframe_destroy (&identity);
        zframe_destroy (&content);
    }
}

void *server_task (void *args)
{
    server_state * state = (server_state*) args;
    
    // Frontend socket talks to clients over TCP
    
    void *frontend = zsocket_new (state->context, ZMQ_ROUTER);
    zsocket_bind (frontend, state->server_url);
    int mandatory = 1;
    zmq_setsockopt(frontend,ZMQ_ROUTER_MANDATORY, &mandatory, sizeof(mandatory));
    
    // Backend socket talks to workers over inproc
    void *backend = zsocket_new (state->context, ZMQ_DEALER);
    zsocket_bind (backend, "inproc://backend");
    
    // Launch pool of worker threads, precise number is not critical
    int thread_nbr;
    for (thread_nbr = 0; thread_nbr < 1; thread_nbr++)
        zthread_fork (state->context, server_worker, state);
    
    // Connect backend to frontend via a proxy
    //zmq_proxy (frontend, backend, NULL);
    zmq_pollitem_t items [] = {
        { frontend, 0, ZMQ_POLLIN, 0 },
        { backend, 0, ZMQ_POLLIN, 0 }
    };
    // Switch messages between sockets
    int counter = 0;
    while (1) {
        ++counter;
        //printf("%d\n",counter);
        //s_dump(frontend);
        zmq_msg_t message;
        zmq_poll (items, 2, -1);
        if (items [0].revents & ZMQ_POLLIN) {
            while (1) {
                //ECHO("SERVER FRONTEND");
                // Process all parts of the message
                zmq_msg_init (&message);
                zmq_msg_recv (&message, frontend, 0);
                int more = zmq_msg_more (&message);
                printf("SERVER MESSAGE IS %s\n", (char*) zmq_msg_data(&message));
                zmq_msg_send (&message, backend, more? ZMQ_SNDMORE: 0);
                zmq_msg_close (&message);
                if (!more)
                    break; // Last message part
            }
        }
        if (items [1].revents & ZMQ_POLLIN) {
            while (1) {
                //ECHO("SERVER BACKAND");
                // Process all parts of the message
                zmq_msg_init (&message);
                zmq_msg_recv (&message, backend, 0);
                int more = zmq_msg_more (&message);
                
                int rc = zmq_msg_send (&message, frontend, more? ZMQ_SNDMORE: 0);
                if(rc != 0)
                {
                    if(errno == EHOSTUNREACH)
                        zmqlog("zmq_msg_send");
                }
                zmq_msg_close (&message);
                
                if (!more)
                {
                    break; // Last message part
                }
            }
        }
    }
    
    zmq_close (frontend);
    zmq_close (backend);
    return NULL;
}

// The main thread simply starts several clients and a server, and then
// waits for the server to finish.

void run_server (const char * server_url)
{
    server_state * state = client_state_init(server_url);
    server_task(state);
    client_state_destroy(state);
}


#endif
