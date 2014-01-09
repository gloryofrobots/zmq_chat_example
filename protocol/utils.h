#ifndef ZCHAT_UTILS_H
#define ZCHAT_UTILS_H

#include "zchat_types.h"

#define LOG_MSG_LENGTH 1024

static
void zchat_log(zchat_const_char_t msg, ...)
{
    //catch var args
    char str [LOG_MSG_LENGTH] = {'\0'};
    va_list argList;
    
    va_start(argList, msg);
    
    int size = vsnprintf(str, LOG_MSG_LENGTH - 1, msg, argList);
    
    va_end(argList);
    
    if( size < 0 )
    {
        return;
    }
    
    printf("%s \n", str);
}
#endif
