
#include <iostream>
#include <fstream>
#include <string>

#include "zmqhelpers.h"

#include <pthread.h>
#include "czmq.h"
#include "zchat_client.h"

int main() 
{
    zchat::run_client("tcp://localhost:5570", "Bob");
}

//static void *
//client_task (void *args)
//{
//    zctx_t *ctx = zctx_new ();
//    void *client = zsocket_new (ctx, ZMQ_DEALER);
//    srand(time(0));
//    // Set random identity to make tracing easier
//    char identity [10];
    
//    sprintf (identity, "%04X-%04X", randof (0x10000), randof (0x10000));
//    zsocket_set_identity (client, identity);
//    zsocket_connect (client, "tcp://localhost:5570");
//    zmq_pollitem_t items [] = { { client, 0, ZMQ_POLLIN, 0 } };
//    int request_nbr = 0;
//    while (true) {
//        // Tick once per second, pulling in arriving messages
//        int centitick;
//        for (centitick = 0; centitick < 100; centitick++) {
//            zmq_poll (items, 1, 10 * ZMQ_POLL_MSEC);
//            if (items [0].revents & ZMQ_POLLIN) {
//                zmsg_t *msg = zmsg_recv (client);
//                zframe_print (zmsg_last (msg), identity);
//                zmsg_destroy (&msg);
//            }
//        }
//        printf("please set input\n");
        
//        char input[256];
//        gets(input);
//        printf("sending %s \n", input);
//        zstr_send (client, "request #%d %s", ++request_nbr, input);
//    }
//    zctx_destroy (&ctx);
//    return NULL;
//}

//    static void *
//client_task (void *args)
//{
//    zctx_t *ctx = zctx_new ();
//    void *client = zsocket_new (ctx, ZMQ_DEALER);
    
//    // Set random identity to make tracing easier
//    char identity [10];
//    srand(time(0));
//    sprintf (identity, "%04X-%04X", randof (0x10000), randof (0x10000));
//    zsocket_set_identity (client, identity);
//    zsocket_connect(client, "tcp://localhost:5570");
//    ECHO2("IDENTITY",identity);
//    zmq_pollitem_t items [] = { { client, 0, ZMQ_POLLIN, 0 } };
//    int request_nbr = 0;
//    while (true) {
//        // Tick once per second, pulling in arriving messages
//        int centitick;
//        for (centitick = 0; centitick < 100; centitick++) {
//            zmq_poll (items, 1, 10 * ZMQ_POLL_MSEC);
//            if (items [0].revents & ZMQ_POLLIN) {
//                zmsg_t *msg = zmsg_recv (client);
//                zframe_print (zmsg_last (msg), identity);
//                zmsg_destroy (&msg);
//            }
//        }
//        zstr_send (client, "request #%d", ++request_nbr);
//    }
//    zctx_destroy (&ctx);
//    return NULL;
//}






//typedef const char * cstring_t;
//struct chat_thread_arguments
//{
//    chat_thread_arguments(void * _context, const char * _remote_url, const char * _send_url, const char * _recv_url)
//        : context(_context)
//        , remote_url(_remote_url)
//        , send_url(_send_url)
//        , recv_url(_recv_url)
//    {
//    }
//    void * context;
//    const char * remote_url;
//    const char * send_url;
//    const char * recv_url;
//};


//void* dealer_task(void * _args){
//    chat_thread_arguments * args = static_cast<chat_thread_arguments *>(_args);
//    void * dealer = zmq_socket(args->context, ZMQ_DEALER);
//    s_set_id (dealer);
//    //void * pull = zmq_socket(args->context, ZMQ_PULL);
//    //void * push = zmq_socket(args->context, ZMQ_PUSH);
//    zmq_connect(dealer, args->remote_url);
////    zmq_bind(pull, args->send_url);
////    zmq_bind(push, args->recv_url);
////    zmq_pollitem_t items [] = { { pull, 0, ZMQ_POLLIN, 0 },
////                                { dealer, 0, ZMQ_POLLIN, 0 }};

//    int rc;
//    while(true)
//    {
//          s_send(dealer,"OLOLO");
//          //cstring_t msg = s_recv(dealer);
////          if(msg)
////          ECHO(msg);

//        /*
//        rc = zmq_poll (items, 2, 1000);
//        if(rc != 0)
//        {
//            zmqlog("dealer_task, zmq_poll");
//            break;
//        }
//       if (items [0].revents & ZMQ_POLLIN)
//        {
//            cstring_t msg = s_recv(dealer);
//             s_send(push, "");
//            s_send(push, msg);
//        }
//        if (items [1].revents & ZMQ_POLLIN)
//        {
//            cstring_t msg = s_recv(pull);
//             s_send(dealer, "");
//            s_send(dealer, msg);
//        }*/
//    }

//    // zmq_close (pull);
//     //zmq_close (push);
//}

//void* sender_task(void * _args)
//{
//    chat_thread_arguments * args = static_cast<chat_thread_arguments *>(_args);
//    void * push = zmq_socket(args->context, ZMQ_PUSH);
//    zmq_connect(push, args->send_url);

//    while(true)
//    {
//         char msg[20] = {'\0'};
//         sprintf(msg,"%d",(int) s_clock());
//         s_send(push, "");
//         s_send(push, msg);
//         sleep(1);
//    }

//    zmq_close (push);
//}

//void* receiver_task(void * _args)
//{
//    chat_thread_arguments * args = static_cast<chat_thread_arguments *>(_args);
//    void * receiver = zmq_socket(args->context, ZMQ_PULL);
//    zmq_connect(receiver, args->recv_url);

//    while(true)
//    {
//         cstring_t msg = s_recv(receiver);
//         if(msg)
//         printf("Received %s\n",msg);
//    }

//    zmq_close (receiver);
//}

//void* echo_task(void * _args)
//{
//    chat_thread_arguments * args = static_cast<chat_thread_arguments *>(_args);
//    void * echo = zmq_socket(args->context, ZMQ_ROUTER);
//    zmq_bind(echo, args->remote_url);

//    while(true)
//    {
//         /*char*  msg = s_recv(echo);
//         if(msg)
//         printf("Echoing %s\n", msg);
//         char msg2[256] = {'\0'};
//         sprintf(msg2,"echo %s",msg);
//         free(msg);
//         s_send(echo, msg2);*/
//         const char * msg = s_recv(echo);
//         msg = s_recv(echo);

//         s_sendmore(echo, "");
//         s_send(echo, "msg2");
//    }

//     zmq_close (echo);
//}



//int main (void)
//{
//    void * context = zmq_ctx_new ();
//    setbuf(stdout,NULL);
//    setbuf(stderr,NULL);

//    chat_thread_arguments args(context, "tcp://127.0.0.1:5555", "inproc://send", "inproc://recv");



//    /*pthread_t echo;
//    pthread_create (&echo, NULL, echo_task, &args);

//    sleep(1);*/

//    pthread_t dealer;
//    pthread_create (&dealer, NULL, dealer_task, &args);

//    void * echo = zmq_socket(args.context, ZMQ_ROUTER);
//    zmq_bind(echo, args.remote_url);

//    while(true)
//    {
//         /*char*  msg = s_recv(echo);
//         if(msg)
//         printf("Echoing %s\n", msg);
//         char msg2[256] = {'\0'};
//         sprintf(msg2,"echo %s",msg);
//         free(msg);
//         s_send(echo, msg2);*/
//         const char * msg = s_recv(echo);
//         msg = s_recv(echo);
//         ECHO(msg);
//         s_sendmore(echo, "");
//         s_send(echo, "msg2");
//    }

//     zmq_close (echo);

//    /*
//    pthread_t sender;
//    pthread_create (&sender, NULL, sender_task, &args);

//    pthread_t receiver;
//    pthread_create (&receiver, NULL, receiver_task, &args);
//    */

//    pthread_join(dealer,0);
//    zmq_ctx_destroy (context);
//}




//    ECHO("!!!!!");
//    pthread_t worker;
//    pthread_create (&worker, NULL, worker_task, context);

//    pthread_join(worker,0);
//    //pthread_timedjoin_np(worker,0,&time_s);
/*
int main (void)
{
    void *context = zmq_ctx_new ();
    
    // First, connect our subscriber socket
    void *subscriber = zmq_socket (context, ZMQ_SUB);
    zmq_connect (subscriber, "tcp://localhost:5561");
    zmq_setsockopt (subscriber, ZMQ_SUBSCRIBE, "", 0);
    
    // 0MQ is so fast, we need to wait a while…
    sleep (1);
    
    // Second, synchronize with publisher
    void *syncclient = zmq_socket (context, ZMQ_REQ);
    zmq_connect (syncclient, "tcp://localhost:5562");
    ECHO("Start client");
    // - send a synchronization request
    s_send (syncclient, "");
    ECHO(" client sync request");
    // - wait for synchronization reply
    char *string = s_recv (syncclient);
    ECHO(" client sync reply");
    free (string);
    
    // Third, get our updates and report how many we got
    int update_nbr = 0;
    while (1) {
        //ECHO(" client get update ");
        //s_send (syncclient, "");
        
        char *string = s_recv (subscriber);
        if (strcmp (string, "END") == 0) {
            free (string);
            break;
        }
        free (string);
        update_nbr++;
    }
    printf ("Received %d updates\n", update_nbr);
    
    zmq_close (subscriber);
    zmq_close (syncclient);
    zmq_ctx_destroy (context);
    return 0;
}
*/

//int main (void)
//{
//    void *context = zmq_ctx_new ();

//    void *socket = zmq_socket (context, ZMQ_DEALER);
//    zmq_setsockopt (socket, ZMQ_IDENTITY, "A", 1);
//    zmq_connect (socket, "tcp://localhost:5671");

//    while(true){
//        // Poll socket for a reply, with timeout
//        zmq_pollitem_t items [] = { { socket, 0, ZMQ_POLLIN, 0 } };
//        int rc = zmq_poll (items, 1, 1000);
//        if (rc == -1)
//        {
//            zmqlog("zmq_poll");
//            break; 
//        }

////        if (items [0].revents & ZMQ_POLLIN)
////        {
//            char *msg = s_recv(socket);
//            if (!msg)
//            {
//                zmqlog("s_recv");
//                break; 
//            }
//            ECHO(msg);
//        //}
//    }
//    zmq_close (socket);
//    zmq_ctx_destroy (context);
//    return 0;
//}
//static void *
//sender_task (void *context)
//{
//    char a_word[255] = {'\0'};

//    printf ("Enter a word: ");
//    scanf ("%s", a_word);
//    printf ("You entered: %s\n", a_word);


//    /*void *worker = zmq_socket (context, ZMQ_DEALER);
//    s_set_id (worker); // Set a printable identity
//    zmq_connect (worker, "tcp://localhost:5671");

//    void * worker2 = zmq_socket (context, ZMQ_DEALER);
//    s_set_id (worker2);
//    zmq_connect (worker2, "tcp://localhost:5671");

//    int total = 0;
//    //s_sendmore(worker,"");
//    //s_send(worker,"");
//    while (1) {
//        // Tell the broker we're ready for work
//        ECHO("s_sendmore");
//        s_sendmore (worker, "");
//        ECHO("s_send");
//        s_send (worker, "Hi Boss");

//        // Get workload from broker, until finished
//        free (s_recv (worker)); // Envelope delimiter
//        char *workload = s_recv (worker);
//        ECHO2("workload", workload);
//        s_sleep (randof (500) + 1);
//    }
//    zmq_close (worker);
//    */
//}
