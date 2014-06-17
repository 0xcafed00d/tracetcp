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

#include "PacketInterface.h"
#include "PacketException.h"

#include <packet32.h>
#include <ntddndis.h>

#pragma comment(lib, "packet.lib")

namespace packet
{
	PacketInterface::PacketInterface() : m_interfaceHandle (NULL)
	{
	}

	PacketInterface::~PacketInterface()
	{
		close();
	}

	void PacketInterface::openLocal (std::string name, int snaplen, int timeout)
	{
        getMACAddress (name, &m_macAddr);
        
        name = 	PCAP_SRC_IF_STRING + name;

		m_interfaceHandle = pcap_open (name.c_str(), snaplen, PCAP_OPENFLAG_PROMISCUOUS, timeout, NULL, m_errorBuffer);
		if (!m_interfaceHandle)
			throw PacketException (m_errorBuffer);
	}

	void PacketInterface::close ()
	{
		if (m_interfaceHandle)
		{
			pcap_close(m_interfaceHandle);
			m_interfaceHandle = NULL;
		}
	}

	void PacketInterface::sendPacket (char* packet, int size)
	{
		if (!m_interfaceHandle)
			throw PacketException ("PacketInterface::sendPacket Failed: Interface not opened");

		if (pcap_sendpacket(m_interfaceHandle, (u_char*)packet, size) != 0)
		{
			throw PacketException (pcap_geterr(m_interfaceHandle));
		}
	}

    //returns false if timeout occurs
    bool PacketInterface::readPacket (pcap_pkthdr **header, const u_char **pkt_data)
    {
		if (!m_interfaceHandle)
			throw PacketException ("PacketInterface::readPacket Failed: Interface not opened");

        int res = pcap_next_ex (m_interfaceHandle, header, pkt_data);

        if (-1 == res)
			throw PacketException ("PacketInterface::readPacket Failed: Error");

        if (0 == res)
            return false;

        return true;
    }


    void PacketInterface::getMACAddress (EthernetAddress* MACAddr)
    {
        *MACAddr = m_macAddr;
    }

	void PacketInterface::getMACAddress (std::string name, EthernetAddress* MACAddr)
	{
		LPADAPTER lpAdapter = PacketOpenAdapter((PCHAR)name.c_str());
	
		if (!lpAdapter || (lpAdapter->hFile == INVALID_HANDLE_VALUE))
		{
			DWORD dwErrorCode = GetLastError();
			throw PacketException ("Unable to open the adapter"); 
		}	

		char OidDataBuffer[6 + sizeof(PACKET_OID_DATA)];
		PPACKET_OID_DATA OidData = (PPACKET_OID_DATA)OidDataBuffer;

		OidData->Oid = OID_802_3_CURRENT_ADDRESS;
		OidData->Length = 6;
		ZeroMemory(OidData->Data, 6);
		
		BOOLEAN status = PacketRequest(lpAdapter, FALSE, OidData);
		if(status)
		{
			memcpy(MACAddr->addr, OidData->Data, 6);
		}
		else
		{
			throw PacketException ("Error retrieving the MAC address of the adapter"); 
		}
		PacketCloseAdapter(lpAdapter);
	}
}