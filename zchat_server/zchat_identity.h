#ifndef ZCHAT_IDENTITY_H
#define ZCHAT_IDENTITY_H
#include "zchat_types.h"
#define IDENTITY_LENGTH 20

struct zchat_identity
{
    zchat_byte_t data[IDENTITY_LENGTH];
    size_t size;
};

zchat_identity * zchat_identity_new(zchat_byte_t* data, size_t size)
{
    if(size >= IDENTITY_LENGTH)
    {
        zchat_log("TOO LARGE IDENTITIY SIZE %d", size);
        return 0;
    }
    
    zchat_identity* identity = new zchat_identity();
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

#endif
