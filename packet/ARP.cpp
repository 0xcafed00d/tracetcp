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

#include "arp.h"

#include "PacketException.h"
#include "..\neo\TimeOut.h"


namespace packet
{
    ARP::ARP(PacketInterface& packetInterface) : m_packetInterface (packetInterface)
    {
    }

    ARP::~ARP()
    {
    }

    void ARP::whoHas (net::InetAddress srcAddr, net::InetAddress queryAddr, EthernetAddress& mac)
    {
        ARPRequest req;

        // fill in ethernet header
        req.etherHeader.destAddr.setToBroadcast();
        m_packetInterface.getMACAddress(&req.etherHeader.srcAddr);
        req.etherHeader.type = ETHERNET_TYPE_ARP;

        // fill in arp packet
        req.arp.clear();
        req.arp.hardType = 1;
        req.arp.protType = 0x800;
        req.arp.hardSize = 6;
        req.arp.protSize = 4;
        req.arp.op = 1;
        req.arp.senderEtherAddr = req.etherHeader.srcAddr;
        req.arp.senderIPAddr = srcAddr.getIPAddress();
        req.arp.targetIPAddr = queryAddr.getIPAddress();

        m_packetInterface.sendPacket((char*) &req, sizeof (req));

        neo::TimeOut timeOut (2000);

        while (true)
        {
            if (timeOut.hasTimedOut())
                throw PacketException ("ARP::whoHas: no response");

            pcap_pkthdr *packetHeader;
            const u_char *packetData;
            m_packetInterface.readPacket (&packetHeader, &packetData);
            
            if (packetHeader->caplen >= sizeof (ARPRequest))
            {
                ARPRequest* resp = (ARPRequest*)packetData;
                if (ETHERNET_TYPE_ARP == resp->etherHeader.type && 
                    2 == resp->arp.op && 
                    req.arp.targetIPAddr == resp->arp.senderIPAddr &&
                    req.arp.senderIPAddr == resp->arp.targetIPAddr)
                {
                    mac = resp->arp.senderEtherAddr;
                    return;
                }
            }
        }
    }
}