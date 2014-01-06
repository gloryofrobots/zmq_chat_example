#ifndef DMSG_LOGGER_H
#define DMSG_LOGGER_H
#include "types.h"
namespace dmsg 
{
    class Logger
    {
    public:
        virtual void log(TRawConstString msg) = 0;
    };
}
#endif
