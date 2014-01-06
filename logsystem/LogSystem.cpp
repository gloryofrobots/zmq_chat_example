#include "LogSystem.h"
#include "Logger.h"
#include <algorithm>
#include <cstdarg>
#include <cstdio>
#include <ctime>
#define LOG_MSG_LENGTH 1024
#define DATE_MSG_LENGTH 20
#define RESULT_LOG_LENGTH LOG_MSG_LENGTH + DATE_MSG_LENGTH
namespace dmsg 
{ 
    void LogSystem::log(TRawConstString msg, ...)
    {
        //catch var args
        char str [LOG_MSG_LENGTH] = {'\0'};
        va_list argList;
        
        va_start(argList, msg);
        
        int size = vsnprintf(str, LOG_MSG_LENGTH - 1, msg, argList);
        
        va_end(argList);
        
        if( size < 0 )
        {
            this->_log("Invalid log message :(\n");
            return;
        }
        
        //add LR and null terminator for correct string
        str[size] = '\n';
        str[size + 1] = 0;	
        
        this->_log(str);
    }
    ////////////////////////////////////////////////////////
    void LogSystem::_log(TRawConstString msg)
    {
        //add current date to log message
        char date[DATE_MSG_LENGTH];
        
        time_t curtime = time (NULL);
        struct tm *loctime = localtime (&curtime);
        strftime (date, 20, "%y-%m-%d %H:%M:%S ", loctime);
        
        //create completed message
        char result [RESULT_LOG_LENGTH] = {'\0'};
        snprintf(result, RESULT_LOG_LENGTH, "%s - %s", date, msg);
        
        //notify loggers
        for( TVectorLogger::iterator 
             it = m_loggers.begin(), 
             it_end = m_loggers.end();
             it != it_end;
             ++it )
        {
            (*it)->log(result);
        }
    }
    ////////////////////////////////////////////////////////
    void LogSystem::addLogger(Logger *logger)
    {
        if( std::find(m_loggers.begin(), m_loggers.end(), logger) != m_loggers.end())
        {
            return;
        }
        
        m_loggers.push_back(logger);
    }
    ////////////////////////////////////////////////////////
}
