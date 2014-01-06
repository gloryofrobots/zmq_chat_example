#ifndef DMSG_SIMPLE_LOGGER_H
#define DMSG_SIMPLE_LOGGER_H
#include "LogSystem.h"

namespace dmsg 
{
    class SimpleLogger : public Logger
    {
    public:
        void log(TRawConstString msg)
        {
            std::cout << msg << std::endl;
        }
    };
}

#endif
