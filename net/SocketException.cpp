//
// Copyright (c) 2003 L.M.Witek
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from
// the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not
//    be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//

#include <winsock.h>
#include "SocketException.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace net
{
    struct ErrorEntry 
    {
        int errorCode;
        const char* errorMessage;

        ErrorEntry(int c, const char* m) 
            :errorCode(c), errorMessage(m) {}
    }; 

    static ErrorEntry g_ErrorList[] = 
    {
        ErrorEntry(WSAEINTR,           "Interrupted system call"),
        ErrorEntry(WSAEBADF,           "Bad file number"),
        ErrorEntry(WSAEACCES,          "Permission denied"),
        ErrorEntry(WSAEFAULT,          "Bad address"),
        ErrorEntry(WSAEINVAL,          "Invalid argument"),
        ErrorEntry(WSAEMFILE,          "Too many open sockets"),
        ErrorEntry(WSAEWOULDBLOCK,     "Operation would block"),
        ErrorEntry(WSAEINPROGRESS,     "Operation now in progress"),
        ErrorEntry(WSAEALREADY,        "Operation already in progress"),
        ErrorEntry(WSAENOTSOCK,        "Socket operation on non-socket"),
        ErrorEntry(WSAEDESTADDRREQ,    "Destination address required"),
        ErrorEntry(WSAEMSGSIZE,        "Message too long"),
        ErrorEntry(WSAEPROTOTYPE,      "Protocol wrong type for socket"),
        ErrorEntry(WSAENOPROTOOPT,     "Bad protocol option"),
        ErrorEntry(WSAEPROTONOSUPPORT, "Protocol not supported"),
        ErrorEntry(WSAESOCKTNOSUPPORT, "Socket type not supported"),
        ErrorEntry(WSAEOPNOTSUPP,      "Operation not supported on socket"),
        ErrorEntry(WSAEPFNOSUPPORT,    "Protocol family not supported"),
        ErrorEntry(WSAEAFNOSUPPORT,    "Address family not supported"),
        ErrorEntry(WSAEADDRINUSE,      "Address already in use"),
        ErrorEntry(WSAEADDRNOTAVAIL,   "Can't assign requested address"),
        ErrorEntry(WSAENETDOWN,        "Network is down"),
        ErrorEntry(WSAENETUNREACH,     "Network is unreachable"),
        ErrorEntry(WSAENETRESET,       "Net connection reset"),
        ErrorEntry(WSAECONNABORTED,    "Software caused connection abort"),
        ErrorEntry(WSAECONNRESET,      "Connection reset by peer"),
        ErrorEntry(WSAENOBUFS,         "No buffer space available"),
        ErrorEntry(WSAEISCONN,         "Socket is already connected"),
        ErrorEntry(WSAENOTCONN,        "Socket is not connected"),
        ErrorEntry(WSAESHUTDOWN,       "Can't send after socket shutdown"),
        ErrorEntry(WSAETOOMANYREFS,    "Too many references, can't splice"),
        ErrorEntry(WSAETIMEDOUT,       "Connection timed out"),
        ErrorEntry(WSAECONNREFUSED,    "Connection refused"),
        ErrorEntry(WSAELOOP,           "Too many levels of symbolic links"),
        ErrorEntry(WSAENAMETOOLONG,    "File name too long"),
        ErrorEntry(WSAEHOSTDOWN,       "Host is down"),
        ErrorEntry(WSAEHOSTUNREACH,    "No route to host"),
        ErrorEntry(WSAENOTEMPTY,       "Directory not empty"),
        ErrorEntry(WSAEPROCLIM,        "Too many processes"),
        ErrorEntry(WSAEUSERS,          "Too many users"),
        ErrorEntry(WSAEDQUOT,          "Disc quota exceeded"),
        ErrorEntry(WSAESTALE,          "Stale NFS file handle"),
        ErrorEntry(WSAEREMOTE,         "Too many levels of remote in path"),
        ErrorEntry(WSAEINVAL,          "One or more parameters are invalid"),
        ErrorEntry(WSASYSNOTREADY,     "Network system is unavailable"),
        ErrorEntry(WSAVERNOTSUPPORTED, "Winsock version out of range"),
        ErrorEntry(WSANOTINITIALISED,  "WSAStartup not yet called"),
        ErrorEntry(WSAEDISCON,         "Graceful shutdown in progress"),
        ErrorEntry(WSAHOST_NOT_FOUND,  "Host not found"),
        ErrorEntry(WSANO_DATA,         "No host data of that type was found"),
        ErrorEntry(-1, 0)
    };

    const char* SocketException::getWSErrorString (int code) const
    {
        ErrorEntry* errs = g_ErrorList;

        for (int n = 0; errs[n].errorCode != -1 ;n++)
            if (errs[n].errorCode == code) 
                return errs[n].errorMessage;

        return "Invalid Error Code";
    }
}