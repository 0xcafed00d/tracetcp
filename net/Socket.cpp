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

#include "Socket.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


namespace net
{
    //********************************************************************
    // Create a Socket object but not internal socket. Socket can be set
    // later by Socket::fromHandle() call
    //********************************************************************
    Socket::Socket ()
    {
        m_socket = INVALID_SOCKET;
        m_valid = false;
    }

    //********************************************************************
    // attach this Socket object to a previously created socket handle
    //********************************************************************
    void Socket::fromHandle (SOCKET s)
    {
        close();
        m_socket = s;
        m_valid = true;
    }

    //********************************************************************
    // Create a socket of a given type.
    // Type can be: SOCK_STREAM
    //              SOCK_DGRAM
    //              SOCK_RAW
    //********************************************************************
    Socket::Socket (int type, int protocol /* = 0*/)
    {
        m_valid = false;

        m_socket = ::socket(PF_INET, type, protocol);
    
        if (m_socket == INVALID_SOCKET)
            processError("Socket::Socket");

        m_valid = true;
    }

    //********************************************************************
    // -- Establishes a connection between socket and peer given by addr.
    //********************************************************************
    void Socket::connect (InetAddress& addr)
    {
        if (::connect(m_socket, (sockaddr*)addr.getSockAddr(), sizeof(sockaddr)) == SOCKET_ERROR)
            processError("Socket::connect");

        // set the remote addr of this connection
        m_remoteAddress = addr;

        // get the local address of the new connection
        sockaddr locaddr; 
        int addrLen = sizeof (addr);
        if (::getsockname(m_socket, &locaddr, &addrLen) != SOCKET_ERROR)
        {
            m_localAddress.setSockAddr((sockaddr_in*)&locaddr);
        }

    }

    // binds the socket to the specified local address
    void Socket::bind (InetAddress& addr)
    {
        if (::bind(m_socket, (sockaddr*)addr.getSockAddr(), sizeof(sockaddr)) == SOCKET_ERROR)
            processError("Socket::bind");

        m_localAddress = addr;
    }

    // sets the socket to listening mode.
    void Socket::listen (int backlog)
    {
        if (::listen (m_socket, backlog) == SOCKET_ERROR)
            processError("Socket::listen");
    }

    // returns the remote address of a connected socket.
    InetAddress Socket::getRemoteAddress ()
    {
        return m_remoteAddress;
    }

    // returns the local address of a connected socket.
    InetAddress Socket::getLocalAddress ()
    {
        return m_localAddress;
    }

    void Socket::setOption (int level, int optionName, int value)
    {
        int ret = ::setsockopt(m_socket, level, optionName, (const char*)&value, sizeof(value));
    
        if (ret == SOCKET_ERROR)
            processError("Socket::setOption");
    }

    void Socket::ioctl (DWORD operation, DWORD value)
    {
        DWORD bytesRet;
        int ret = WSAIoctl(m_socket, operation, &value, sizeof(value), NULL, 0 , &bytesRet, NULL, NULL);

        if (ret == SOCKET_ERROR)
            processError("Socket::ioctl");
    }

    // accepts a new connection and returns a new Socket for this connection
    Socket::ref Socket::accept ()
    {
        sockaddr_in addr;          // remote address
        sockaddr_in locaddr;       // local address
        int addrLen = sizeof (addr);

        SOCKET newSock = ::accept(m_socket, (sockaddr*)&addr, &addrLen);

        if (newSock == INVALID_SOCKET)
        {
            // processError will throw an exception
            processError("Socket::accept");
        }

        // set up the new socket 
        Socket::ref s = new Socket();
        s->fromHandle(newSock);
        s->m_remoteAddress.setSockAddr((sockaddr_in*)&addr);

        // get the local address of the new socket
        if (::getsockname(newSock, (sockaddr*)&locaddr, &addrLen) != SOCKET_ERROR)
        {
            s->m_localAddress.setSockAddr((sockaddr_in*)&locaddr);
        }
        return s;
    }

    int Socket::send (const char *buffer, int len, int flags /* =0 */)
    {
        int numsent = ::send(m_socket, buffer, len, flags);

        if (numsent == SOCKET_ERROR)
            processError("Socket::send");

        return numsent;
    }

    int Socket::sendTo (const InetAddress& addr, const char *buffer, int len, int flags /* =0 */)
    {
        int numsent = ::sendto(m_socket, buffer, len, flags, (const sockaddr*)addr.getSockAddr(), sizeof (sockaddr_in));

        if (numsent == SOCKET_ERROR)
            processError("Socket::send");

        return numsent;
    }


    int Socket::recv (char *buffer, int len, int flags /* =0 */)
    {
        int numrec = ::recv(m_socket, buffer, len, flags);
        if (numrec == SOCKET_ERROR)
            processError("Socket::recv");

        return numrec;
    }

    
    int Socket::recvFrom (InetAddress& addr, char *buffer, int len, int flags /* =0 */)
    {
        sockaddr_in saddr;
        int saddr_sz = sizeof (sockaddr_in);

        int numrec = ::recvfrom(m_socket, buffer, len, flags, (sockaddr*)&saddr, &saddr_sz);
        if (numrec == SOCKET_ERROR)
            processError("Socket::recv");

        addr.setSockAddr(&saddr);

        return numrec;
    }



    int Socket::send (const neo::MemoryBlock& mem, int flags /* =0 */)
    {
        return send(mem.getBlock(), (int)mem.getSize(), flags);
    }

    int Socket::sendTo (const InetAddress& addr, const neo::MemoryBlock& mem, int flags /* =0 */)
    {
        return sendTo (addr, mem.getBlock(), (int)mem.getSize(), flags);
    }

    int Socket::recv (neo::MemoryBlock& mem, int flags /* =0 */)
    {
        return recv(mem.getBlock(), (int)mem.getSize(), flags);
    }

    int Socket::recvFrom (InetAddress& addr, neo::MemoryBlock& mem, int flags /* =0 */)
    {
        return recvFrom(addr, mem.getBlock(), (int)mem.getSize(), flags);
    }

    int Socket::send (const std::string& str, int flags /* =0 */)
    {
        return send(str.data(), (int)str.length(), flags);
    }

    int Socket::sendTo (const InetAddress& addr, const std::string& str, int flags /* =0 */)  
    {
        return sendTo (addr, str.data(), (int)str.length(), flags);
    }

    int Socket::recv (std::string& str, int flags /* =0 */)
    {
        const int bufSize = 512;
        char buffer[bufSize];

        int numrec = recv(buffer, bufSize, flags);

        str = std::string(buffer, numrec);

        return numrec;
    }

    int Socket::recvFrom (InetAddress& addr, std::string& str, int flags /* =0 */)
    {
        const int bufSize = 512;
        char buffer[bufSize];

        int numrec = recvFrom(addr, buffer, bufSize, flags);

        str = std::string(buffer, numrec);

        return numrec;   
    }

	// blocks until there is data to read on the socket of timeout occurs
	// returns true if data to read els false if timeout occurs.
    bool Socket::isReadable (u_long timeout_ms)
    {
        struct timeval to;
        fd_set fds;

        fds.fd_count = 1;
        fds.fd_array[0] = m_socket;
        to.tv_sec = timeout_ms/1000;
        to.tv_usec = (timeout_ms - to.tv_sec * 1000) * 1000;

        int ret = ::select(0, &fds, NULL, NULL, &to);

        if (ret == SOCKET_ERROR)
            processError("Socket::isReadable");

        return ret != 0;
    }


    // close the socket - wibble....
    void Socket::close ()
    {
        int retVal = 0;
        if (m_valid)
        {
            retVal = closesocket(m_socket);
            m_socket = INVALID_SOCKET;
            m_valid = false;

            if (retVal == SOCKET_ERROR)
                processError("Socket::close");
        }
    }

    Socket::~Socket()
    {
        try
        {
            close();
        }
        catch (...)
        {
            // ignore any errors when closing as we cant throw from a dtor.
        }
    }

    void Socket::processError (const char* functionName)
    {
         throw SocketException(functionName, WSAGetLastError());
    }

}


