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

#if !defined(AFX_SOCKET_H__C3C380B9_9054_4371_ABD8_1116324F53F1__INCLUDED_)
#define AFX_SOCKET_H__C3C380B9_9054_4371_ABD8_1116324F53F1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif 


#include "InetAddress.h"
#include "SocketException.h"
#include "InitWinsock.h"
#include "..\neo\RefPtr.h"
#include "..\neo\MemoryBlock.h"

namespace net
{
    class Socket  
    {
        public: 
            typedef neo::RefPtr<Socket> ref;

            // Construction & Destruction
            Socket ();
            Socket (int type, int protocol = 0);
            virtual ~Socket();
            void fromHandle (SOCKET s);
            void close ();

            // socket manipulation 
            void connect (InetAddress& addr);
            void bind (InetAddress& addr);
            void listen (int backlog = SOMAXCONN);
            Socket::ref accept ();
            void setOption (int level, int optionName, int value);
            void ioctl (DWORD operation, DWORD value);

            // status info
            bool isReadable (u_long timeout_ms);

            // transfer directly from/to memory
            int send     (const char *buffer, int len, int flags = 0); 
            int sendTo   (const InetAddress& addr, const char *buffer, int len, int flags = 0);        
            int recv     (char *buffer, int len, int flags = 0);
            int recvFrom (InetAddress& addr, char *buffer, int len, int flags = 0);

            // transfer from/to MemoryBlock object
            int send     (const neo::MemoryBlock& mem, int flags = 0);        
            int sendTo   (const InetAddress& addr, const neo::MemoryBlock& mem, int flags = 0);        
            int recv     (neo::MemoryBlock& mem, int flags = 0);
            int recvFrom (InetAddress& addr, neo::MemoryBlock& mem, int flags = 0);

            // transfer from/to str::string object
            int send     (const std::string& str, int flags = 0);  
            int sendTo   (const InetAddress& addr, const std::string& str, int flags = 0);        
            int recv     (std::string& str, int flags = 0);
            int recvFrom (InetAddress& addr, std::string& str, int flags = 0);

            // queries
            InetAddress getRemoteAddress ();
            InetAddress getLocalAddress ();

        private:
            // private copy ctor and assignment op to prevent copies. Sockets 
            // cannot be copied and must be passed by ref or pointer
            Socket (const Socket&);
            Socket& operator= (const Socket& that);
            
            SOCKET m_socket;
            
            InetAddress m_remoteAddress;
            InetAddress m_localAddress;
            bool m_valid;

            void processError (const char* functionName);
    };
}
#endif 
