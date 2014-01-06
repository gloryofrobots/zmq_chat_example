/* =====================================================================
 z helpers.h                       *
 
 Helper header file for example applications.
 =====================================================================
 */

#ifndef __ZHELPERS_H_INCLUDED__
#define __ZHELPERS_H_INCLUDED__

// Include a bunch of headers that we will need in the examples

#include <zmq.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <assert.h>
#include <signal.h>
#include <uuid/uuid.h>

// Version checking, and patch up missing constants to match 2.1
#if ZMQ_VERSION_MAJOR == 2
# error "Please upgrade to ZeroMQ/3.2 for these examples"
#endif

// Provide random number from 0..(num-1)
#if (defined (__WINDOWS__))
# define randof(num) (int) ((float) (num) * rand () / (RAND_MAX + 1.0))
#else
# define randof(num) (int) ((float) (num) * random () / (RAND_MAX + 1.0))
#endif


void zmqlog(const char * msg)
{
    printf("%s %s\n", msg, zmq_strerror(zmq_errno()));
}

#define ECHO(s) printf("%s\n",s)
#define ECHO2(s1,s2) printf("%s %s\n",s1,s2)

// Receive 0MQ string from socket and convert into C string
// Caller must free returned string. Returns NULL if the context
// is being terminated.
static char *
s_recv (void *socket) {
    char buffer [256];
    int size = zmq_recv (socket, buffer, 255, 0);
    if (size == -1)
    {
        return NULL;
        zmqlog("");
    }
    if (size > 255)
        size = 255;
    buffer [size] = 0;
    return strdup (buffer);
}

// Convert C string to 0MQ string and send to socket
static int
s_send (void *socket,const char *string) {
    int size = zmq_send (socket, string, strlen (string), 0);
    return size;
}
// Convert C string to 0MQ string and send to socket
static int
s_send_const (void *socket,const char *string) {
;
    int size = zmq_send_const (socket, string, strlen (string), 0);
    return size;
}

// Sends string as 0MQ string, as multipart non-terminal
static int
s_sendmore (void *socket, const char *string) {
    int size = zmq_send (socket, string, strlen (string), ZMQ_SNDMORE);
    return size;
}

// Receives all message parts from socket, prints neatly
//
static void
s_dump (void *socket)
{
    puts ("----------------------------------------");
    while (1) {
        // Process all parts of the message
        zmq_msg_t message;
        zmq_msg_init (&message);
        int size = zmq_msg_recv (&message, socket, 0);
        
        // Dump the message as text or binary
        char *data = (char *) zmq_msg_data (&message);
        int is_text = 1;
        int char_nbr;
        for (char_nbr = 0; char_nbr < size; char_nbr++)
            if ((unsigned char) data [char_nbr] < 32
                || (unsigned char) data [char_nbr] > 127)
                is_text = 0;
            
            printf ("[%03d] ", size);
        for (char_nbr = 0; char_nbr < size; char_nbr++) {
            if (is_text)
                printf ("%c", data [char_nbr]);
            else
                printf ("%02X", (unsigned char) data [char_nbr]);
        }
        printf ("\n");
        
        int64_t more; // Multipart detection
        more = 0;
        size_t more_size = sizeof (more);
        zmq_getsockopt (socket, ZMQ_RCVMORE, &more, &more_size);
        zmq_msg_close (&message);
        if (!more)
            break; // Last message part
    }
}

// Set simple random printable identity on socket
//
static void
s_set_id (void *socket)
{
    char identity [10];
    sprintf (identity, "%04X-%04X", randof (0x10000), randof (0x10000));
    zmq_setsockopt (socket, ZMQ_IDENTITY, identity, strlen (identity));
}


// Sleep for a number of milliseconds
static void
s_sleep (int msecs)
{
    #if (defined (__WINDOWS__))
    Sleep (msecs);
    #else
    struct timespec t;
    t.tv_sec = msecs / 1000;
    t.tv_nsec = (msecs % 1000) * 1000000;
    nanosleep (&t, NULL);
    #endif
}




// Return current system clock as milliseconds
static int64_t
s_clock (void)
{
    #if (defined (__WINDOWS__))
    SYSTEMTIME st;
    GetSystemTime (&st);
    return (int64_t) st.wSecond * 1000 + st.wMilliseconds;
    #else
    struct timeval tv;
    gettimeofday (&tv, NULL);
    return (int64_t) (tv.tv_sec * 1000 + tv.tv_usec / 1000);
    #endif
}


// Sleep for a number of milliseconds
static void
s_sleep2 (int msecs)
{
    int64_t start = s_clock();
    while(1)
    {
        int64_t check = s_clock();
        int64_t delta = check - start;
        if(delta > msecs) return;
    }
}


// Print formatted string to stdout, prefixed by date/time and
// terminated with a newline.

static void
s_console (const char *format, ...)
{
    time_t curtime = time (NULL);
    struct tm *loctime = localtime (&curtime);
    char *formatted = (char *) malloc (20);
    strftime (formatted, 20, "%y-%m-%d %H:%M:%S ", loctime);
    printf ("%s", formatted);
    free (formatted);
    
    va_list argptr;
    va_start (argptr, format);
    vprintf (format, argptr);
    va_end (argptr);
    printf ("\n");
}


static int s_interrupted = 0;
static void s_signal_handler (int signal_value)
{
    s_interrupted = 1;
}

static void s_raise()
{
    raise(SIGINT);
}

static void s_catch_signals (void)
{
    struct sigaction action;
    action.sa_handler = s_signal_handler;
    action.sa_flags = 0;
    sigemptyset (&action.sa_mask);
    sigaction (SIGINT, &action, NULL);
    sigaction (SIGTERM, &action, NULL);
}

#endif // __ZHELPERS_H_INCLUDED__