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


#include <iostream>

#include "WinpcapPacketInterface.h"
#include "packet\ARP.h"
#include "packet\iputils.h"
#include "packet\InterfaceEnumerator.h"

using namespace std;

WinpcapPacketInterface::WinpcapPacketInterface()
{
}

WinpcapPacketInterface::~WinpcapPacketInterface()
{
}

void WinpcapPacketInterface::overrideGateway (net::InetAddress gwaddress)
{
	m_overridegateway = gwaddress;
}

void WinpcapPacketInterface::initialise (net::InetAddress target)
{
	m_target = target;
	bool targetIsLocal = false;

	// find the route to target host.
	packet::getRouteInfo(target, m_source, m_gateway);
	if (m_source.getIPAddress() == m_gateway.getIPAddress())
	{
		targetIsLocal = true;
	}
	else
	{
		if (m_overridegateway.getIPAddress() != 0)
			m_gateway = m_overridegateway;
	}

	// find the local interface that hase the source address
	packet::InterfaceEnumerator ie;
	ie.findLocalInterfaces();

	std::string interfaceName;

	for (size_t n = 0; n < ie.getInterfaceCount(); n++)
	{
		const pcap_if_t& iface = ie.getInterface(n);
		packet::ListAccessor<pcap_addr> addresses (iface.addresses);
		for (size_t addrIndex = 0; addrIndex < addresses.getListLength(); addrIndex++)
		{
			pcap_addr* address = addresses.at(addrIndex);
			net::InetAddress inetAddr(address->addr);

			if (inetAddr.getIPAddress() == m_source.getIPAddress())
			{
				interfaceName = iface.name;	
			}
		}
	}

	if (interfaceName != "")
	{
		//cout << "Using interface: " << interfaceName << endl;

		m_packetif.openLocal(interfaceName, 2048, 1);
		m_packetif.getMACAddress(&m_sourceMAC);
		
		packet::ARP arp(m_packetif);

		// if target is not local we need to send packet to gateway.
		if (targetIsLocal)
			arp.whoHas(m_source, m_target, m_destinationMAC);
		else
			arp.whoHas(m_source, m_gateway, m_destinationMAC);
		
		// make an ethernet header to send packets with
		m_ethernetHeader.clear();
		m_ethernetHeader.type = ETHERNET_TYPE_IP;
		m_ethernetHeader.srcAddr = m_sourceMAC;
		m_ethernetHeader.destAddr = m_destinationMAC;
	}
}

net::InetAddress WinpcapPacketInterface::getSourceAddress ()
{
	return m_source;
}

void WinpcapPacketInterface::sendPacket (const char *buffer, int len)
{
	neo::MemoryBlock packetBuffer (sizeof (m_ethernetHeader) + len);
	memcpy (packetBuffer.getBlock(), &m_ethernetHeader, sizeof (m_ethernetHeader));
	memcpy (packetBuffer.getBlock()+sizeof (m_ethernetHeader), buffer, len);

	m_packetif.sendPacket(packetBuffer.getBlock(), packetBuffer.getSize());
}

bool WinpcapPacketInterface::recPacket (neo::MemoryBlock& buffer, net::InetAddress& from, int timeout)
{
	pcap_pkthdr *packetHeader;
	const u_char *packetData;

	if (m_packetif.readPacket (&packetHeader, &packetData))
	{
		if (packetHeader->caplen >= ((sizeof(packet::EthernetHeader) + sizeof(packet::IPHeader))))
		{
			packet::EthernetHeader* header = (packet::EthernetHeader*)packetData;
			packet::IPHeader* ipheader = (packet::IPHeader*)(packetData + sizeof(packet::EthernetHeader));
			if (header->srcAddr == m_destinationMAC && header->destAddr == m_sourceMAC && header->type == ETHERNET_TYPE_IP)
			{
				memcpy (buffer.getBlock(), packetData + sizeof(packet::EthernetHeader),  packetHeader->caplen - sizeof(packet::EthernetHeader));
				from.setIPAddress(ipheader->sourceIP.get());
				return true;
			}
		}
	}

	return false;
}
