set(MY_LIB_NAME logsystem)

set(LIB_SRC_LIST 
    "LogSystem.h"
    "Logger.h"
    "LogSystem.cpp"
    "SimpleLogger.h"
 )
 
ADD_LIBRARY( ${MY_LIB_NAME} ${LIB_SRC_LIST} )
