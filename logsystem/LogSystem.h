#ifndef DMSG_LOGSYSTEM_H
#define DMSG_LOGSYSTEM_H
#include "types.h"
#include "Holder.h"
#include "Logger.h"
#include <vector>
#include <iostream>
#define DMSG_LOGGER dmsg::LogSystem::get()->log

namespace dmsg 
{
    class LogSystem : public Holder<LogSystem>
    {
    public:
        void log(TRawConstString msg, ...);    
        
        void addLogger(Logger * logger);
    private:
        void _log(TRawConstString msg);    
        typedef std::vector<Logger *> TVectorLogger;
        TVectorLogger m_loggers;
    };
}

#endif
