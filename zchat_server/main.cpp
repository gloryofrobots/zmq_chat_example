#include <iostream>
#include <fstream>
#include "zmqhelpers.h"
#include "types.h"
#include <pthread.h>
#include <vector>


#include "zchat_server.h"
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
    
//    Message message;
//    message.set_id(1234);
//    message.set_type(Message_MessageType_HEARTBEAT);
//    std::fstream output("myfile", std::ios::out | std::ios::binary | std::ios::trunc);
//    message.SerializeToOstream(&output);
//    output.close();
//    std::fstream input("myfile", std::ios::in | std::ios::binary);
//    Message message2;
//    message2.ParseFromIstream(&input);
//    std::cout << "id: " << message2.id() << std::endl;
//    std::cout << "type: " << message2.type() << std::endl;
//    chat::async();
    zchat::async();
    return 0;
}


