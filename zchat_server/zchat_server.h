#ifndef ZCHAT_SERVER_H
#define ZCHAT_SERVER_H
#include "czmq.h"
#include "zmqhelpers.h"


//Asynchronous zchat_server. USE DEALER AND ROUTER


#include "czmq.h"
#include "zmqhelpers.h"
// This is our client task
// It connects to the server, and then sends a request once per second
// It collects responses as they arrive, and it prints them out. We will
// run several client tasks in parallel, each with a different random ID.
namespace zchat
{
    struct zchat_client_state
    {
        const char * server_url;
    };
    
    zchat_client_state* init_state()
    {
        srand(time(0));
        zchat_client_state * state = (zchat_client_state *) malloc(sizeof(zchat_client_state));
        // Set random identity to make tracing easier
        state->server_url = "tcp://localhost:5570";
    }
    
    void destroy_state(zchat_client_state* state)
    {
        free(state);
    }



   
    static void server_worker (void *args, zctx_t *ctx, void *pipe);
    
    void *server_task (void *args)
    {
        // Frontend socket talks to clients over TCP
        zctx_t *ctx = zctx_new ();
        void *frontend = zsocket_new (ctx, ZMQ_ROUTER);
        zsocket_bind (frontend, "tcp://*:5570");
        int mandatory = 1;
        zmq_setsockopt(frontend,ZMQ_ROUTER_MANDATORY, &mandatory, sizeof(mandatory));
        
        // Backend socket talks to workers over inproc
        void *backend = zsocket_new (ctx, ZMQ_DEALER);
        zsocket_bind (backend, "inproc://backend");
        
        // Launch pool of worker threads, precise number is not critical
        int thread_nbr;
        for (thread_nbr = 0; thread_nbr < 1; thread_nbr++)
            zthread_fork (ctx, server_worker, NULL);
        
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
                        break; // Last message part
                }
            }
        }
        
        zmq_close (frontend);
        zmq_close (backend);
        zctx_destroy (&ctx);
        return NULL;
    }
    
    // Each worker task works on one request at a time and sends a random number
    // of replies back, with random delays between replies:
    
    static void
    server_worker (void *args, zctx_t *ctx, void *pipe)
    {
        void *worker = zsocket_new (ctx, ZMQ_DEALER);
        zsocket_connect (worker, "inproc://backend");
        
        while (true) {
            // The DEALER socket gives us the reply envelope and message
            zmsg_t *msg = zmsg_recv (worker);
            zframe_t *identity = zmsg_pop (msg);
            zframe_t *content = zmsg_pop (msg);
            assert (content);
            zmsg_destroy (&msg);
            
            // Send 0..4 replies back
            int reply, replies = randof (5);
            for (reply = 0; reply < 1; reply++) {
                // Sleep for some fraction of a second
                zclock_sleep (randof (1000) + 1);
                zframe_send (&identity, worker, ZFRAME_REUSE + ZFRAME_MORE);
                zframe_send (&content, worker, ZFRAME_REUSE);
            }
            zframe_destroy (&identity);
            zframe_destroy (&content);
        }
    }
    
    // The main thread simply starts several clients and a server, and then
    // waits for the server to finish.
    
    void async (void)
    {
//        zthread_new (client_task, NULL);
//        zthread_new (client_task, NULL);
//        zthread_new (client_task, NULL);
//        zthread_new (server_task, NULL);
        server_task(0);
        //zclock_sleep (5 * 1000); // Run for 5 seconds then quit
    }
    
}
#endif