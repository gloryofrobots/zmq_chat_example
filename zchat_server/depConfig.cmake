    

SET ( ZEROMQ_FIND_REQUIRED TRUE )

## PROTOBUF

find_package(Protobuf)
if(PROTOBUF_FOUND)
 MESSAGE ( STATUS "Found Protobuf:" )
endif(PROTOBUF_FOUND)

### ZeroMQ ###
   
FIND_LIBRARY ( PROTOBUF_LIBRARY NAMES protobuf PATHS /usr/lib /usr/local/lib )
 
FIND_PATH ( ZEROMQ_INCLUDE_DIR NAMES zmq.h PATHS /usr/include/ /usr/local/include/ )
FIND_LIBRARY ( ZEROMQ_LIBRARY NAMES zmq PATHS /usr/lib /usr/local/lib )

FIND_PATH ( CZEROMQ_INCLUDE_DIR NAMES czmq.h PATHS /usr/include/ /usr/local/include/ )
FIND_LIBRARY ( CZEROMQ_LIBRARY NAMES czmq PATHS /usr/lib /usr/local/lib )

IF ( ZEROMQ_INCLUDE_DIR AND ZEROMQ_LIBRARY )
    SET ( ZEROMQ_FOUND TRUE )
ENDIF ( ZEROMQ_INCLUDE_DIR AND ZEROMQ_LIBRARY )
    
    
IF ( ZEROMQ_FOUND )
    SET ( DEP_FOUND TRUE )
    IF ( NOT ZEROMQ_FIND_QUIETLY )
        MESSAGE ( STATUS "Found ZeroMQ:" )
        MESSAGE ( STATUS "  (Headers)       ${ZEROMQ_INCLUDE_DIR}" )
        MESSAGE ( STATUS "  (Library)       ${ZEROMQ_LIBRARY}" )
    ENDIF ( NOT ZEROMQ_FIND_QUIETLY )
ELSE ( ZEROMQ_FOUND )
    SET ( DEP_FOUND FALSE )
    IF ( ZEROMQ_FIND_REQUIRED )
        MESSAGE ( FATAL_ERROR "Could not find ZeroMQ" )
    ENDIF ( ZEROMQ_FIND_REQUIRED )
ENDIF ( ZEROMQ_FOUND )

