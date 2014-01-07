#ifndef ZCHAT_CLIENT_H
#define ZCHAT_CLIENT_H
#include "czmq.h"
#include "zmqhelpers.h"
#include "message.pb.h"
#define LINE_SIZE 255
#define IDENTITY_SIZE 20
namespace zchat
{
    /////////////////////////////////////////////////////////////////////
    struct server_state
    {
        zctx_t* context;
        char identity[IDENTITY_SIZE];
        const char * server_url;
        const char * login;
        int last_message_id;
    };
    /////////////////////////////////////////////////////////////////////
    server_state* init_state(const char* server_url, const char* login)
    {
        srand(time(0));
        server_state * state = (server_state *) malloc(sizeof(server_state));
        state->context = zctx_new ();
        state->login = login;
        
        sprintf (state->identity, "%04X-%04X", randof (0x10000), randof (0x10000));
        state->server_url = server_url;
        // Set random identity to make tracing easier
    }
    /////////////////////////////////////////////////////////////////////
    void destroy_state(server_state* state)
    {
        ECHO("DESTROYED");
        zctx_destroy (&state->context);
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
    /////////////////////////////////////////////////////////////////////
    //read stdin
    static void 
    worker_task (void *args, zctx_t *ctx, void *pipe)
    {
        server_state* state = (server_state*) args;
        
        void *worker = zsocket_new (ctx, ZMQ_DEALER);
        zsocket_connect (worker, "inproc://backend");
        
        while (true) {
            printf("please set input\n");
            
            char input[LINE_SIZE] = {'\0'};
            get_line(input, LINE_SIZE);
            
            printf("sending %s \n", input);
            Message message;
            
            message.set_id(state->last_message_id);
            state->last_message_id++;
            message.set_type(Message_MessageType_MESSAGE);
            message.set_value(input);
            message.set_login(state->login);
            std::string serialisedMessage;
            message.SerializeToString(&serialisedMessage);
            
            ECHO(message.ShortDebugString().c_str());
            //zframe_t* identity = zframe_new (state->identity, IDENTITY_SIZE);
            zframe_t* content = zframe_new (serialisedMessage.c_str(),
                                             serialisedMessage.length());
            
            zclock_sleep (randof (1000) + 1);
            //zframe_send (&identity, worker, ZFRAME_REUSE + ZFRAME_MORE);
            zframe_send (&content, worker, ZFRAME_REUSE);
            
            //zframe_destroy (&identity);
            zframe_destroy (&content);
        }
    }
    /////////////////////////////////////////////////////////////////////
    static void *
    client_task (void *args)
    {
        server_state* state = (server_state*) args;
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
                    zmq_msg_init (&zmessage);
                    zmq_msg_recv (&zmessage, frontend, 0);
                    int more = zmq_msg_more (&zmessage);
                    if(more){
                        printf("GET MORE  \n");
                    }
                    //zmsg_dump(&message);
                    
                    //zmq_msg_send (&message, backend, more? ZMQ_SNDMORE: 0);
                    
                    
                    char * data = (char *) zmq_msg_data(&zmessage);
                    zmq_msg_close (&zmessage);
                    if (!more){
                        Message message;
                        message.ParseFromArray(data,strlen(data));
                        ECHO2("RECEIVED", message.ShortDebugString().c_str());
                        //printf("MESSAGE IS %s\n", data);  
                        
                        break; // Last message part
                    }              
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
                    
                    ECHO("SENDING");
                    int rc = zmq_msg_send (&zmessage, frontend, more? ZMQ_SNDMORE: 0);
                    if(rc == -1)
                    {
                        zmqlog("zmq_msg_send");
                    }

                    zmq_msg_close (&zmessage);
                    if (!more)
                        break; // Last message part
                }
            }
        }
        
        zctx_destroy (&state->context);
        return NULL;
    }
    /////////////////////////////////////////////////////////////////////
    void run_client(const char* server_url, const char* login)
    {
        server_state * state = init_state(server_url, login);
        
        client_task(state);
       
        destroy_state(state);
    }
}
#endif
