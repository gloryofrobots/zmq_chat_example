#include <iostream>
#include "zmqhelpers.h"
#include "SimpleLogger.h"
#include "LogSystem.h"
#include "types.h"
#include <pthread.h>
#include <vector>
#include "ex.h"
#include "async.h"
#include "async2.h"
typedef std::vector<void *> TVectorWorker;


int main()
{
    setbuf(stdout,NULL);
    setbuf(stderr,NULL);
    ///timespec time_s = {time(0) + 3,0};
    //pthread_join(worker,0);
    //pthread_timedjoin_np(worker,0,&time_s);
    
    void *context = zmq_ctx_new ();
    //req_router(context);
    chat::async();
    return 0;
}


