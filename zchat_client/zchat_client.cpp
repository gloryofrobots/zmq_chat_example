#include <string>
#include <pthread.h>


#include "zmqhelpers.h"

#include "zchat_client.h"
#include "zchat_message.h"
#include "zchat_types.h"
#include "utils.h"

#include <list>
#include <vector>
#define LINE_SIZE 255
#define IDENTITY_SIZE 20

#define HEARTBEAT_LIVENESS 3 // 3-5 is reasonable
#define HEARTBEAT_INTERVAL 1000 // msecs
#define HEARTBEAT_INTERVAL_INIT 1000 // Initial reconnect
#define HEARTBEAT_INTERVAL_MAX 32000 

/////////////////////////////////////////////////////////////////////
struct client_state
{
    zctx_t* context;
    char identity[IDENTITY_SIZE];
    const char * server_url;
    const char * login;
    zchat_message_vector_t in_messages;
    zchat_message_vector_t out_messages;
    zchat_vector_string_t users;
    
    size_t heartbeat_liveness;
    size_t heartbeat_interval;
    size_t heartbeat_interval_init;
    size_t heartbeat_interval_max;
    uint64_t heartbeat_time;
    int last_message_id;
};
void client_state_reset_heartbeat(client_state* state);
/////////////////////////////////////////////////////////////////////
client_state* client_state_init(const char* server_url, const char* login)
{
    srand(time(0));
    client_state * state = new client_state();
    state->last_message_id = 0;
    state->context = zctx_new ();
    state->login = login;
    
    // Set random identity to make tracing easier
    sprintf (state->identity, "%04X-%04X", randof (0x10000), randof (0x10000));
    
    state->server_url = server_url;
    
    client_state_reset_heartbeat(state);
    return state;
}
/////////////////////////////////////////////////////////////////////
void client_state_destroy(client_state* state)
{
    ECHO("DESTROYED");
    zctx_destroy (&state->context);
    delete state;
}
/////////////////////////////////////////////////////////////////////
void client_state_reset_heartbeat(client_state* state)
{
    state->heartbeat_liveness = HEARTBEAT_LIVENESS;
    state->heartbeat_interval_max = HEARTBEAT_INTERVAL_MAX;
    state->heartbeat_interval_init = HEARTBEAT_INTERVAL_INIT;
    state->heartbeat_interval = HEARTBEAT_INTERVAL;  
}
/////////////////////////////////////////////////////////////////////
void client_state_set_heartbeat_time(client_state* state)
{
    state->heartbeat_time =  zclock_time () + state->heartbeat_interval;     
}
/////////////////////////////////////////////////////////////////////
bool client_state_is_time_to_heartbeat(client_state* state)
{
    return zclock_time() > state->heartbeat_interval;
}
/////////////////////////////////////////////////////////////////////
zchat_message * zchat_message_new_for_sender(client_state* state, zchat_message_message_type type)
{
    zchat_message* message = zchat_message_new();
    message->set_type(type);
    message->set_sender(state->login);
    message->set_incoming_id(++state->last_message_id);
}
/////////////////////////////////////////////////////////////////////
//FGETS without line separator    
void get_line(char * input, int size)
{
    fgets(input, size, stdin);
    char * temp = input;
    while (temp)
    {
        if(*temp == '\n')
        {
            *temp = '\0';
            break;
        }
        
        temp++;
    }    
}
/////////////////////////////////////////////////////////////////////
static void
serialize_message_to_string(zchat_message * message, zchat_string_t* out)
{
    message->SerializeToString(out);
}
/////////////////////////////////////////////////////////////////////
void send_outgoing_messages(client_state* state, void * socket)
{
    for(zchat_message_vector_t::iterator 
        it = state->out_messages.begin();
        it != state->out_messages.end();
        it++)
    {
        zchat_string_t serialised;
        zchat_message * message = *it;
        
        serialize_message_to_string(message, &serialised);
        zframe_t* content = zframe_new (serialised.c_str(),
                                        serialised.length());
        
        zclock_sleep (randof (1000) + 1);
        
        zframe_send (&content, socket, ZFRAME_REUSE);
        if(message->type() == zchat_message_message_type_PING)
        {
            client_state_set_heartbeat_time(state);
        }
        
        zframe_destroy (&content);
        zchat_message_destroy(message);
    }
    
    state->out_messages.clear();
}
/////////////////////////////////////////////////////////////////////
void add_outgoing_message(client_state* state, zchat_message * message)
{
    state->out_messages.push_back(message);
}
/////////////////////////////////////////////////////////////////////
void add_incoming_message(client_state* state, zchat_message * message)
{
    state->in_messages.push_back(message);
}
/////////////////////////////////////////////////////////////////////
void process_backend_message(client_state* state, zmq_msg_t* zmessage)
{
    zchat_message * message = zchat_message_deserialize_from_zmq_msg(zmessage);
    add_outgoing_message(state, message);
    //ECHO_2_STR("process_backend_message", message->ShortDebugString().c_str());
}
/////////////////////////////////////////////////////////////////////
void process_frontend_message(client_state* state, zmq_msg_t* zmessage)
{
    zchat_message * message = zchat_message_deserialize_from_zmq_msg(zmessage);
    add_incoming_message(state, message);
    //ECHO_2_STR("process_frontend_message", message->ShortDebugString().c_str());
}
/////////////////////////////////////////////////////////////////////
void process_pong_message(client_state* state, zchat_message * message)
{
    int size = message->users_size();
    state->users.clear();
    
    for(int i = 0; i < size; i++)
    {
        const zchat_string_t& user = message->users(0);
        state->users.push_back(user);
    }
}
/////////////////////////////////////////////////////////////////////
void add_ping_message(client_state* state)
{
    zchat_message* message = zchat_message_new_for_sender(state, zchat_message_message_type_PING);
    add_outgoing_message(state, message);
    state->heartbeat_liveness--;
}
/////////////////////////////////////////////////////////////////////
void add_pong_message(client_state* state)
{
    zchat_message* message = zchat_message_new_for_sender(state, zchat_message_message_type_PONG);
    add_outgoing_message(state, message);
}
/////////////////////////////////////////////////////////////////////
static void get_serialised_message_from_stdin(client_state* state, zchat_string_t* data)
{   
    char input[LINE_SIZE] = {'\0'};
    
    get_line(input, LINE_SIZE);
    
    zchat_message * message = zchat_message_new_for_sender(state, zchat_message_message_type_MESSAGE);
    
    message->set_value(input);
    //message.set_value("OLOLOLOLOLO");
    message->SerializeToString(data);
    //const char * ds = message->ShortDebugString().c_str();
    //ECHO(ds);
    zchat_message_destroy(message);
}
/////////////////////////////////////////////////////////////////////
zframe_t* get_frame_from_stdin(client_state* state)
{
    zchat_string_t serialisedMessage;
    get_serialised_message_from_stdin(state, &serialisedMessage);
    zframe_t* content = zframe_new (serialisedMessage.c_str(),
                                    serialisedMessage.length());
    
    //const char * data = serialisedMessage.c_str();
    return content;
}
/////////////////////////////////////////////////////////////////////
static void worker_task (void *args, zctx_t *ctx, void *pipe)
{
    // Send out heartbeats at regular intervals
    client_state* state = (client_state*) args;
    void *worker = zsocket_new (ctx, ZMQ_DEALER);
    zsocket_connect (worker, "inproc://backend");
    
    while (true) {
        ECHO("please set input");
        
        zframe_t* content = get_frame_from_stdin(state);
        
        zclock_sleep (randof (1000) + 1);
        
        zframe_send (&content, worker, ZFRAME_REUSE);
        zframe_destroy (&content);
    }
}
/////////////////////////////////////////////////////////////////////
static void 
client_loop_frontend (client_state* state, void *frontend)
{
    zmq_msg_t zmessage;
    while (1) {
        // Process all parts of the message
        //printf("RECEIVED  \n");
        
        zmq_msg_init (&zmessage);
        zmq_msg_recv (&zmessage, frontend, 0);
        
        int more = zmq_msg_more (&zmessage);
        
        if(more)
        {
            zmq_msg_close (&zmessage);      
            continue;
        }
        
        zchat_message* message = zchat_message_deserialize_from_zmq_msg(&zmessage);
        if(message == NULL)
        {
            zchat_log("Message deserialisation error");
            zmq_msg_close (&zmessage);
            return;
        }
        
        if(message->type() == zchat_message_message_type_PONG)
        {
            process_pong_message(state, message);
        }
        
        if(message->type() == zchat_message_message_type_PING)
        {
            client_state_reset_heartbeat(state);
            client_state_set_heartbeat_time(state);
            add_pong_message(state);
        }
        
        client_state_reset_heartbeat(state);
        client_state_set_heartbeat_time(state);
        ECHO_2_STR("RECEIVED", message->ShortDebugString().c_str());
        
        zchat_message_destroy(message);
        zmq_msg_close (&zmessage);
        break; // Last message part
    }
}
/////////////////////////////////////////////////////////////////////
static void 
client_loop_backend(client_state* state, void *backend)
{
    zmq_msg_t zmessage;
    while (1)
    {
        // Process all parts of the message
        zmq_msg_init (&zmessage);
        zmq_msg_recv (&zmessage, backend, 0);
        int more = zmq_msg_more (&zmessage);
        
        if(more)
        {
            zmq_msg_close (&zmessage);
            continue;
        }
        
        //char * data = (char *) zmq_msg_data(&zmessage);
        //printf("sending %s\n", data);
        
        process_backend_message(state, &zmessage);
        zmq_msg_close (&zmessage);
        break;
    }
}
/////////////////////////////////////////////////////////////////////
static void 
client_loop (client_state* state, void *frontend, void *backend)
{
    zmq_pollitem_t items [] = { { frontend, 0, ZMQ_POLLIN, 0 },
                                { backend, 0, ZMQ_POLLIN, 0 } };
    
    client_state_set_heartbeat_time(state);
    while (1) {
        if(state->heartbeat_liveness <= 0)
        {
            //we dead
            ECHO("Connection offline.");
            return;
        }
        
        //s_dump(frontend);
        
        zmq_poll (items, 2, -1);
        if (items [0].revents & ZMQ_POLLIN)
        {
            client_loop_frontend(state,frontend);      
        }
        if (items [1].revents & ZMQ_POLLIN)
        {
            client_loop_backend(state, backend);           
        }
        
        if(client_state_is_time_to_heartbeat(state) == true)
        {
            add_ping_message(state);
        }
        
        send_outgoing_messages(state, frontend);
    }
}
static void 
client_task (void *args)
{
    client_state* state = (client_state*) args;
    void *frontend = zsocket_new (state->context, ZMQ_DEALER);
    zsocket_set_identity (frontend, state->identity);
    zsocket_connect (frontend,state->server_url);
    
    void *backend = zsocket_new (state->context, ZMQ_DEALER);
    zsocket_bind (backend, "inproc://backend");
    zthread_fork (state->context, worker_task, state);
    
    client_loop(state, frontend, backend);
    
    zsocket_destroy(state->context, &frontend);
    zsocket_destroy(state->context, &backend);
    zctx_destroy (&state->context);
}
/////////////////////////////////////////////////////////////////////
void zchat_client_run(const char* server_url, const char* login)
{
    client_state * state = client_state_init(server_url, login);
    
    client_task(state);
    
    client_state_destroy(state);
}

