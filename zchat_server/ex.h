#ifndef EX_H
#define EX_H
#include <iostream>
#include <vector>
#include "zmqhelpers.h"
#include <pthread.h>

void* req_task(void * args)
{
    static int s_id = 0;
    int id = s_id++;
    
    void * context = args;
    void *requester = zmq_socket (context, ZMQ_REQ);
    zmq_connect (requester, "tcp://localhost:5555");
    
    int counter = 1;
    while (counter++) {
        char buffer [10];
        printf ("Sending Hello %d %d…\n", id, counter);
        zmq_send (requester, "Hello", 5, 0);
        zmq_recv (requester, buffer, 10, 0);
        printf ("Received World %d %d %s\n", id, counter, buffer);
    }
    zmq_close (requester);
    zmq_ctx_destroy (context);
    return 0;
}

void req_rep(void *context)
{
    void *responder = zmq_socket (context, ZMQ_REP);
    
    zmq_bind (responder, "tcp://*:5555");
    pthread_t worker;
    pthread_create (&worker, NULL, req_task, context);
    pthread_t worker2;
    pthread_create (&worker2, NULL, req_task, context);
    
    while (1) {
        char buffer [10];
        zmq_recv (responder, buffer, 10, 0);
        printf ("Received Hello\n");
        //sleep (1); // Do some 'work'
        zmq_send (responder, "World", 5, 0);
    }
}

////////////////////////////////////////////////////////////////////////////

static void *
dealer_task (void *args)
{
    static int s_id = 0;
    int id = s_id++;
    
    void *context = args;
    void *worker = zmq_socket (context, ZMQ_DEALER);
    s_set_id (worker); // Set a printable identity
    zmq_connect (worker, "tcp://localhost:5555");
    
    int counter = 1;
    while (counter++) {
        
        printf("worker s_send %d\n", counter);
        s_send (worker, "Hi Boss");
        ECHO("worker s_recv");
        char * msg = s_recv(worker);
        printf("worker received: %d %d %s\n",id, counter, msg);
        free(msg);
        
        printf("worker s_send2 %d\n", counter);
        s_send (worker, "Buy Boss");
        
        s_sleep (randof (500) + 1000);
    }
    
    zmq_close (worker);
}

int dealer_router (void *context)
{
    void *router = zmq_socket (context, ZMQ_ROUTER);
    
    zmq_bind (router, "tcp://*:5555");
    srandom ((unsigned) time (NULL));
    
    pthread_t dealer;
    pthread_create (&dealer, NULL, dealer_task, context);
    
    pthread_t dealer1;
    pthread_create (&dealer1, NULL, dealer_task, context);
    
    while (1) {
        zmq_pollitem_t items [] = { { router, 0, ZMQ_POLLIN, 0 } };
        zmq_poll (items, 1, 100 * 1000);
        if (items [0].revents & ZMQ_POLLIN) {
            char *identity = s_recv (router);
            ECHO2("IDENTITY ",identity);
           
            char *msg = s_recv (router);
            ECHO2("MSG ",msg);
            
            ECHO("sending to worker ");
            s_sendmore (router, identity);
            free (identity);
            free (msg);
            s_send(router, "Work harder");
           
            identity = s_recv (router);
            ECHO2("IDENTITY2 ",identity);
           
            msg = s_recv (router);
            ECHO2("MSG2 ",msg);
            
            free (identity);
            free (msg);
        }
    }
    
    zmq_close (router);
    zmq_ctx_destroy (context);
    return 0;
}



int req_router (void *context)
{
    void *router = zmq_socket (context, ZMQ_ROUTER);
    
    zmq_bind (router, "tcp://*:5555");
    srandom ((unsigned) time (NULL));
    
    pthread_t dealer;
    pthread_create (&dealer, NULL, dealer_task, context);
    
    /*pthread_t dealer1;
    pthread_create (&dealer1, NULL, dealer_task, context);*/
    
    while (1) {
        char *identity = s_recv (router);
        ECHO2("IDENTITY ",identity);
        
        char *msg = s_recv (router);
        ECHO2("MSG ",msg);
        
        ECHO("sending to worker ");
        s_sendmore (router, identity);
        
        free (identity);
        free (msg);
        s_send(router, "Work harder");
    }
    
    zmq_close (router);
    zmq_ctx_destroy (context);
    return 0;
}

/////////////////////////////////////////////////////////////







#endif
