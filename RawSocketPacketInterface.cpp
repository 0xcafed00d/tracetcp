//
// Copyright (c) 2005 L.M.Witek
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

#include "RawSocketPacketInterface.h"

RawSocketPacketInterface::RawSocketPacketInterface() : m_socket (NULL)
{
}

RawSocketPacketInterface::~RawSocketPacketInterface()
{
}


void RawSocketPacketInterface::overrideGateway (net::InetAddress gwaddress)
{
	// do nothing we cant override gateway with raw socket interface
}


net::InetAddress RawSocketPacketInterface::getSourceAddress ()
{
    if (m_source.getIPAddress() == 0)
    {
        m_target.setPortNumber(123); 
        net::Socket s(SOCK_DGRAM);
        s.connect(m_target);
        m_source = s.getLocalAddress();
    }
    return m_source;
}

void RawSocketPacketInterface::initialise (net::InetAddress target)
{
    m_target = target;

    // get the local ip to fill in header
    net::InetAddress local = getSourceAddress();

    m_socket = new net::Socket (SOCK_RAW, IPPROTO_IP);
    m_socket->setOption (IPPROTO_IP, IP_HDRINCL, TRUE);
    m_socket->bind (local);
    m_socket->ioctl (SIO_RCVALL, true);
}

void RawSocketPacketInterface::sendPacket (const char *buffer, int len)
{
    m_socket->sendTo (m_target, buffer, len);
}

bool RawSocketPacketInterface::recPacket (neo::MemoryBlock& buffer, net::InetAddress& from, int timeout)
{
    if (m_socket->isReadable(timeout))
    {
        m_socket->recvFrom(from, buffer);
        return true;
    }
    return false;
}
