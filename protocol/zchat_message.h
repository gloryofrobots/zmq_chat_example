#ifndef MESSAGE_UTILS_H
#define MESSAGE_UTILS_H
#include "czmq.h"
#include "zchat_message.pb.h"

typedef  std::vector<zchat_message*> zchat_message_vector_t;
///////////////////////////////////////////
zchat_message * zchat_message_new()
{
    return new zchat_message();
}
///////////////////////////////////////////
void zchat_message_destroy(zchat_message * message)
{
    delete message;
}
///////////////////////////////////////////
zchat_message * zchat_message_deserialize_from_zmq_msg(zmq_msg_t* zmessage)
{
    int size = zmq_msg_size(zmessage);
    void * data = zmq_msg_data(zmessage);
    
    zchat_message* message = zchat_message_new();
    message->ParseFromArray(data, size);
    return message;
}
///////////////////////////////////////////
zchat_message * zchat_message_deserialize_from_zframe(zframe_t *content)
{
    zchat_message* message = new zchat_message();
    byte * data = zframe_data(content);
    size_t size = zframe_size(content);
    message->ParseFromArray(data, size);
    return message;
}
///////////////////////////////////////////
#endif
