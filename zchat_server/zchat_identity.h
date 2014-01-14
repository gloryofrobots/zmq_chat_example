#ifndef ZCHAT_IDENTITY_H
#define ZCHAT_IDENTITY_H
#include "zchat_types.h"
#include "czmq.h"

#define IDENTITY_LENGTH 20
#define LIVENESS 3
#define PING_INTERVAL 1000
struct zchat_identity
{
    zchat_byte_t data[IDENTITY_LENGTH];
    size_t size;
    
    size_t liveness;
    uint64_t ping_time;
};

void zchat_identity_reset_liveness(zchat_identity* identity);
zchat_identity * zchat_identity_new(zchat_byte_t* data, size_t size)
{
    if(size >= IDENTITY_LENGTH)
    {
        zchat_log("TOO LARGE IDENTITIY SIZE %d", size);
        return 0;
    }
    
    zchat_identity* identity = new zchat_identity();
    zchat_identity_reset_liveness(identity);
    
    memcpy(identity->data, data, size);
    identity->size = size;
    return identity;
}

void zchat_identity_destroy(zchat_identity* identity)
{
    delete identity;
}

bool zchat_identity_is_equals(zchat_identity* identity, zchat_byte_t * data, size_t size)
{
    if(size != identity->size)
    {
         zchat_log("IDENTITY SIZE ERROR %d != %d", size, identity->size);
         return false;
    }
    
    return memcmp(identity->data, data, size) == 0;
}
/////////////////////////////////////////////////////////////////////
void zchat_identity_set_ping_time(zchat_identity* identity)
{
    identity->ping_time =  zclock_time () + PING_INTERVAL;
}
/////////////////////////////////////////////////////////////////////
bool zchat_identity_is_time_to_ping(zchat_identity* identity)
{
    return zclock_time() > identity->ping_time;
}
/////////////////////////////////////////////////////////////////////
void zchat_identity_reset_liveness(zchat_identity* identity)
{
    identity->liveness = LIVENESS;
    zchat_identity_set_ping_time(identity); 
}
/////////////////////////////////////////////////////////////////////
void zchat_identity_decrease_liveness(zchat_identity* identity)
{
    identity->liveness--; 
}
/////////////////////////////////////////////////////////////////////
bool zchat_identity_is_alive(zchat_identity* identity)
{
    return identity->liveness >= 0; 
}
/////////////////////////////////////////////////////////////////////
#endif
