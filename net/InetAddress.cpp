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

#include "InetAddress.h"

#include <stdio.h>
#include "..\neo\StringUtils.h"


namespace net
{
    // --------------------------------------------------------------------------
    // -- constructors 
    // --------------------------------------------------------------------------

    InetAddress::InetAddress()
    {
        clear();
    }

    // handle addressed of the format: "www.news.com:80" or "192.168.0.1:smtp"
    InetAddress::InetAddress(const char* _hostname)
    {
        std::string hostname(_hostname);
        std::string::size_type colonPos = hostname.find(':');
        if (std::string::npos == colonPos)
        {
            // no port specified - use 0
            InetAddress addr (_hostname, 0);
            m_address = addr.m_address;
        }
        else
        {
            std::string portStr = hostname.substr (colonPos + 1);
            hostname = hostname.substr (0, colonPos);
            
            try
            {
                unsigned short portNum = neo::parseUnsignedShort (portStr);
                InetAddress addr (hostname.c_str(), portNum);
                m_address = addr.m_address;
            }
            catch (neo::ParseException)
            {
                // the port is not a number - look it up as a service name 
                InetAddress addr (hostname.c_str(), portStr.c_str(), "tcp");
                m_address = addr.m_address;
            }
        }
    }

    InetAddress::InetAddress(const char* hostname, unsigned short port)
    {
        m_address.sin_family      = AF_INET;          
        m_address.sin_addr.s_addr = hostToAddr(hostname);  
        m_address.sin_port        = htons(port);      
    }

    InetAddress::InetAddress(const char* hostname, const char* service, const char* protocol)
    {
        servent* s = getservbyname(service, protocol);
        if (!s)
        {
            processError("InetAddress::getservbyname");
        }

        m_address.sin_family      = AF_INET;          
        m_address.sin_addr.s_addr = hostToAddr(hostname);  
        m_address.sin_port        = s->s_port;      
    }


    InetAddress::InetAddress(unsigned long addrIP, unsigned short port)
    {
        m_address.sin_family      = AF_INET;          
        m_address.sin_addr.s_addr = htonl(addrIP);  
        m_address.sin_port        = htons(port);      
    }

    InetAddress::InetAddress(const sockaddr* addr)
    {
        m_address = *(const sockaddr_in*)addr;
    }

    InetAddress::~InetAddress()
    {
    }


    // --------------------------------------------------------------------------
    // -- helpers 
    // --------------------------------------------------------------------------

    unsigned long InetAddress::hostToAddr (const char* hostname) const
    {
        // convert a.b.c.d notation to ip address
        unsigned long ipaddr = inet_addr(hostname);
        if (ipaddr == INADDR_NONE)
        {
            // failed. try convert from hostname to ip address
            hostent* h = gethostbyname(hostname);
            if (!h)
            {
                processError("InetAddress::hostToAddr");
            }

            ipaddr = *((unsigned long*) h->h_addr_list[0]);
        }

        return ipaddr;
    }

    void InetAddress::processError (const char* functionName) const
    {
         throw SocketException(functionName, WSAGetLastError());
    }

    // --------------------------------------------------------------------------
    // -- Utilities
    // --------------------------------------------------------------------------

    void InetAddress::clear ()
    {
        memset(&m_address, 0, sizeof(m_address)); 
        m_address.sin_family = AF_INET;          
    }

    // --------------------------------------------------------------------------
    // -- Accessors
    // --------------------------------------------------------------------------

    unsigned long InetAddress::getIPAddress () const
    {
        return ntohl (m_address.sin_addr.s_addr);
    }

    std::string InetAddress::getIPAddressAsString () const
    {
        char ipstr[32];
        sprintf (ipstr, "%d.%d.%d.%d", m_address.sin_addr.S_un.S_un_b.s_b1,
                                       m_address.sin_addr.S_un.S_un_b.s_b2,
                                       m_address.sin_addr.S_un.S_un_b.s_b3,
                                       m_address.sin_addr.S_un.S_un_b.s_b4);

        return std::string(ipstr);
    }

    unsigned short InetAddress::getPortNumber () const
    {
        return ntohs (m_address.sin_port);
    }

    void InetAddress::setIPAddress (unsigned long ip)
    {
        m_address.sin_addr.s_addr = htonl(ip);  
    }

    void InetAddress::setPortNumber (unsigned short port)
    {
        m_address.sin_port = htons(port);      
    }

    void InetAddress::set (unsigned long ip, unsigned short port)
    {
        setIPAddress(ip);
        setPortNumber(port);
    }

    std::string InetAddress::lookupHostName () const
    {
        hostent* h = gethostbyaddr((const char*)&m_address.sin_addr.s_addr, 4, AF_INET);
        if (!h)
        {
            processError("InetAddress::lookupHostName");
        }

        return h->h_name;
    }


    // --------------------------------------------------------------------------
    // -- Adaptors to external structures
    // --------------------------------------------------------------------------

    const sockaddr_in* InetAddress::getSockAddr () const
    {
        return &m_address;
    }

    void InetAddress::setSockAddr (sockaddr_in* addr)
    {
        m_address = *addr;
    }


}