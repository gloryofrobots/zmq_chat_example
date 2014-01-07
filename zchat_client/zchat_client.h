#ifndef ZCHAT_CLIENT_H
#define ZCHAT_CLIENT_H
#include "czmq.h"
#include "zmqhelpers.h"
#define LINE_SIZE 255
#define IDENTITY_SIZE 20

struct zchat_client_state
{
    zctx_t* context;
    char identity[IDENTITY_SIZE];
    const char * server_url;
};

zchat_client_state* init_state()
{
    srand(time(0));
    zchat_client_state * state = (zchat_client_state *) malloc(sizeof(zchat_client_state));
    // Set random identity to make tracing easier
    sprintf (state->identity, "%04X-%04X", randof (0x10000), randof (0x10000));
    state->server_url = "tcp://localhost:5570";
}

void destroy_state(zchat_client_state* state)
{
    free(state);
}


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
worker_task (void *args, zctx_t *ctx, void *pipe)
{
    zchat_client_state* state = (zchat_client_state*) args;

    void *worker = zsocket_new (ctx, ZMQ_DEALER);
    zsocket_connect (worker, "inproc://backend");
    
    while (true) {
        printf("please set input\n");
        
        char input[LINE_SIZE] = {'\0'};
        get_line(input, LINE_SIZE);
        
        printf("sending %s \n", input);
        
        //zframe_t* identity = zframe_new (state->identity, IDENTITY_SIZE);
        zframe_t* content = zframe_new (input, LINE_SIZE);
        
        zclock_sleep (randof (1000) + 1);
        //zframe_send (&identity, worker, ZFRAME_REUSE + ZFRAME_MORE);
        zframe_send (&content, worker, ZFRAME_REUSE);
        
        //zframe_destroy (&identity);
        zframe_destroy (&content);
    }
}

//static void *
//input_task(void *)
//{
//    void *worker = zsocket_new (ctx, ZMQ_DEALER);
//    zsocket_connect (worker, "inproc://backend");
//    z_
//}


static void *
client_task (void *args)
{
    zchat_client_state* state = (zchat_client_state*) args;
    
    state->context = zctx_new ();
    
    
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
        zmq_msg_t message;
        zmq_poll (items, 2, -1);
        if (items [0].revents & ZMQ_POLLIN) {
            while (1) {
                // Process all parts of the message
                zmq_msg_init (&message);
                zmq_msg_recv (&message, frontend, 0);
                int more = zmq_msg_more (&message);
                if(more){
                     printf("GET MORE  \n");
                }
                //zmsg_dump(&message);
                
                //zmq_msg_send (&message, backend, more? ZMQ_SNDMORE: 0);
                
                
                char * data = (char *) zmq_msg_data(&message);
                zmq_msg_close (&message);
                if (!more){
                    printf("MESSAGE IS %s\n", data);  
                      
                     break; // Last message part
                     }              
            //zmsg_t *msg = zmsg_recv (frontend);
//                zframe_print (zmsg_last (msg), state->identity);
            }
        }
        if (items [1].revents & ZMQ_POLLIN) {
            while (1) {
                // Process all parts of the message
                zmq_msg_init (&message);
                zmq_msg_recv (&message, backend, 0);
                int more = zmq_msg_more (&message);
                
                ECHO("SENDING");
                int rc = zmq_msg_send (&message, frontend, more? ZMQ_SNDMORE: 0);
                if(rc != 0)
                {
                    if(errno == EHOSTUNREACH)
                        zmqlog("zmq_msg_send");
                }
                zmq_msg_close (&message);
                if (!more)
                    break; // Last message part
            }
        }
    }
    zctx_destroy (&state->context);
    return NULL;
}
void zchat_client_main()
{
    zchat_client_state * state = init_state();
   
    client_task(state);
    ECHO("DESTROYED");
    destroy_state(state);
}

#endif
