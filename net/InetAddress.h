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

#if !defined(___INETADDRESS_H_INCLUDED_)
#define ___INETADDRESS_H_INCLUDED_

#include <string>
#include <iostream>
#include <winsock2.h>
#include <Mstcpip.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

#include "SocketException.h"

namespace net
{
    // wrapper around the sockaddr_in structure.
    class InetAddress  
    {
        private:
            sockaddr_in m_address;
            
            // -- helpers --
            void processError (const char* functionName) const;
            unsigned long hostToAddr (const char* hostname) const;

        public:
            
            // -- constructors --
            InetAddress();
            InetAddress(const char* hostname);
            InetAddress(const char* hostname, unsigned short port);
            InetAddress(const char* hostname, const char* service, const char* protocol);
            InetAddress(unsigned long addrIP, unsigned short port);
            InetAddress(const sockaddr* addr);
            virtual ~InetAddress();

            // -- Utilities --
            void clear ();

            // -- Accessors -- 
            unsigned long getIPAddress () const;
            std::string getIPAddressAsString () const;
            unsigned short getPortNumber () const;

            std::string lookupHostName () const;

            void setIPAddress (unsigned long ip);
            void setPortNumber (unsigned short port);
            void set (unsigned long ip, unsigned short port);

            // -- Adaptors to external structures --
            const sockaddr_in* getSockAddr () const;
            void setSockAddr (sockaddr_in* addr);
    };

    inline std::ostream& operator<< (std::ostream& os, const InetAddress& addr)
    {
        return os << addr.getIPAddressAsString();
    }
}

#endif 
