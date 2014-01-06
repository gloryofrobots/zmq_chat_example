#ifndef DMSGTYPES_H
#define DMSGTYPES_H
#include <string>
namespace dmsg 
{
    typedef unsigned int TUInt;
    typedef char TChar;
    typedef TChar * TRawString;
    typedef const TChar * TRawConstString;
    typedef std::string TString;
    typedef long long TTimeStamp;
}

#endif
