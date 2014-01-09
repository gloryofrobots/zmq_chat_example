#include <string>
#include <pthread.h>


#include "zmqhelpers.h"

#include "zchat_client.h"
#include "zchat_message.h"
#include "zchat_types.h"
#include <list>
#include <vector>
#define LINE_SIZE 255
#define IDENTITY_SIZE 20



/////////////////////////////////////////////////////////////////////
struct client_state
{
    zctx_t* context;
    char identity[IDENTITY_SIZE];
    const char * server_url;
    const char * login;
    zchat_message_vector_t* in_messages;
    zchat_message_vector_t* out_messages;
    
    int last_message_id;
};
/////////////////////////////////////////////////////////////////////
client_state* init_state(const char* server_url, const char* login)
{
    srand(time(0));
    client_state * state = (client_state *) malloc(sizeof(client_state));
    state->last_message_id = 0;
    state->context = zctx_new ();
    state->login = login;
    state->in_messages = new zchat_message_vector_t();
    state->out_messages = new zchat_message_vector_t();
    // Set random identity to make tracing easier
    sprintf (state->identity, "%04X-%04X", randof (0x10000), randof (0x10000));
    state->server_url = server_url;
    return state;
    
}
/////////////////////////////////////////////////////////////////////
void destroy_state(client_state* state)
{
    ECHO("DESTROYED");
    zctx_destroy (&state->context);
    delete state->in_messages;
    delete state->out_messages;
    free(state);
}

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

static void
set_message_id(client_state* state, zchat_message& message)
{
    message.set_incoming_id(++state->last_message_id);
    //state->last_message_id++;
}

static void
serialize_message_to_string(zchat_message * message, zchat_string_t* out)
{
    message->SerializeToString(out);
}

void send_outgoing_messages(client_state* state, void * socket)
{
    
    for(zchat_message_vector_t::iterator 
        it = state->out_messages->begin();
        it != state->out_messages->end();
        it++)
    {
        zchat_string_t serialised;
        zchat_message * message = *it;
        serialize_message_to_string(message, &serialised);
        zframe_t* content = zframe_new (serialised.c_str(),
                                        serialised.length());
        
        zclock_sleep (randof (1000) + 1);
        
        zframe_send (&content, socket, ZFRAME_REUSE);
        zframe_destroy (&content);
        
        /*ECHO("SENDING");
                int rc = zmq_msg_send (&zmessage, frontend, more? ZMQ_SNDMORE: 0);
                if(rc == -1)
                {
                    zmqlog("zmq_msg_send");
                }*/
    }
    
    state->out_messages->clear();
}
/////////////////////////////////////////////////////////////////////
void add_outgoing_message(client_state* state, zchat_message * message)
{
    state->out_messages->push_back(message);
}
/////////////////////////////////////////////////////////////////////
void add_incoming_message(client_state* state, zchat_message * message)
{
    state->in_messages->push_back(message);
}
/////////////////////////////////////////////////////////////////////
void process_backend_message(client_state* state, zmq_msg_t* zmessage)
{
    zchat_message * message = zchat_message_deserialize_from_zmq_msg(zmessage);
    add_outgoing_message(state, message);
    ECHO_2_STR("process_backend_message", message->ShortDebugString().c_str());
    
}
/////////////////////////////////////////////////////////////////////
void process_frontend_message(client_state* state, zmq_msg_t* zmessage)
{
    zchat_message * message = zchat_message_deserialize_from_zmq_msg(zmessage);
    add_incoming_message(state, message);
    ECHO_2_STR("process_frontend_message", message->ShortDebugString().c_str());
}
/////////////////////////////////////////////////////////////////////
static void
get_serialised_message_from_stdin(client_state* state, zchat_string_t* data)
{   
    char input[LINE_SIZE] = {'\0'};
    
    get_line(input, LINE_SIZE);
    
    zchat_message message;
    set_message_id(state, message);
    
    message.set_type(zchat_message_message_type_MESSAGE);
    message.set_value(input);
    //message.set_value("OLOLOLOLOLO");
    message.set_sender(state->login);
    message.SerializeToString(data);
    const char * ds = message.ShortDebugString().c_str();
    ECHO(ds);
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
//read stdin
static void 
worker_task (void *args, zctx_t *ctx, void *pipe)
{
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
static void *
client_task (void *args)
{
    client_state* state = (client_state*) args;
    void *frontend = zsocket_new (state->context, ZMQ_DEALER);
    zsocket_set_identity (frontend, state->identity);
    zsocket_connect (frontend,state->server_url);
    
    void *backend = zsocket_new (state->context, ZMQ_DEALER);
    zsocket_bind (backend, "inproc://backend");
    zthread_fork (state->context, worker_task, state);
    
    zmq_pollitem_t items [] = { { frontend, 0, ZMQ_POLLIN, 0 },
                                { backend, 0, ZMQ_POLLIN, 0 } };
    
    int counter = 0;
    while (1) {
        ++counter;
        
        
        //printf("%d\n",counter);
        //s_dump(frontend);
        zmq_msg_t zmessage;
        zmq_poll (items, 2, -1);
        if (items [0].revents & ZMQ_POLLIN) {
            while (1) {
                // Process all parts of the message
                printf("RECEIVED  \n");
                zmq_msg_init (&zmessage);
                zmq_msg_recv (&zmessage, frontend, 0);
                int more = zmq_msg_more (&zmessage);
                if(more){
                    printf("GET MORE  \n");
                }
                //zmsg_dump(&message);
                
                //zmq_msg_send (&message, backend, more? ZMQ_SNDMORE: 0);
                
                if (!more){
                    zchat_message message;
                    char * data = (char *) zmq_msg_data(&zmessage);
                    zmq_msg_close (&zmessage);
                    message.ParseFromArray(data, strlen(data));
                    ECHO_2_STR("RECEIVED", message.ShortDebugString().c_str());
                    //printf("MESSAGE IS %s\n", data);  
                    
                    break; // Last message part
                }     
                zmq_msg_close (&zmessage);         
                //zmsg_t *msg = zmsg_recv (frontend);
                //                zframe_print (zmsg_last (msg), state->identity);
            }
        }
        if (items [1].revents & ZMQ_POLLIN) {
            while (1) {
                // Process all parts of the message
                zmq_msg_init (&zmessage);
                zmq_msg_recv (&zmessage, backend, 0);
                int more = zmq_msg_more (&zmessage);
                
                if(more)
                {
                    continue;
                }
                //char * data = (char *) zmq_msg_data(&zmessage);
                //printf("sending %s\n", data);
                
                process_backend_message(state, &zmessage);
                zmq_msg_close (&zmessage);
                break;
            }
        }
        
        send_outgoing_messages(state, frontend);
        
    }
    
    zctx_destroy (&state->context);
    return NULL;
}

/////////////////////////////////////////////////////////////////////
void zchat_client_run(const char* server_url, const char* login)
{
    client_state * state = init_state(server_url, login);
    
    client_task(state);
    
    destroy_state(state);
}

