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

#include <cstdlib>
#include "packet\PacketDefs.h"
#include "net\socket.h"
#include "neo\TimeOut.h"
#include "tcptrace.h"
#include "RawSocketPacketInterface.h"
#include "WinpcapPacketInterface.h"

using namespace packet;

enum ResponsePacketTypes
{
    NOT_VALID,       // the packet was not a response to something we sent.
    ICMP_ECHO_RESP,  // ping response
    ICMP_TTL_EXP,    // packet expired in transit.
    ICMP_NO_ROUTE,   // no route to host
    ICMP_SOURCE_QU,
	ICMP_REDIRECT,
    TCP_RST,
    TCP_SYNACK
};

// Compares what we sent to what we recieved back an determines if the packet is in reply to what we sent
ResponsePacketTypes validateResponse(TCPIPHeader& sent, neo::MemoryBlock& resp)
{
    IPHeader* responseIP = (IPHeader*)(resp.getBlock());
    if (responseIP->protocol == IPPROTO_ICMP)
    {
        // the response is ICMP - so find out the type of the response
        ICMPHeader* icmpheader = (ICMPHeader*)(resp.getBlock() + responseIP->getHeaderSize());

        switch (icmpheader->type)
        {
            case ICMPHeader::DEST_UNREACH:
            case ICMPHeader::TTL_EXPIRED:
            case ICMPHeader::SOURCE_QUENCH:
			case ICMPHeader::REDIRECT:
            {
                ICMPErrorHeader* errorRep = (ICMPErrorHeader*)icmpheader;
                IPHeader* returnedIP = &(errorRep->ipHeader);

                // recover the the fist 64 bits of the original packet.
                TCPHeader* returnedTCP = (TCPHeader*)(resp.getBlock() + responseIP->getHeaderSize() + errorRep->getHeaderSize());
                // only src port, dest port & seq num will be present in the returned packet.            

                if (sent.ip.id == returnedIP->id &&
                    sent.ip.destIP == returnedIP->destIP &&
                    sent.tcp.destPort == returnedTCP->destPort &&
                    sent.tcp.seqNum == returnedTCP->seqNum)
                {
                    // the error contained the packet we sent
                    switch (icmpheader->type)
                    {
                        case ICMPHeader::DEST_UNREACH:  return ICMP_NO_ROUTE;
                        case ICMPHeader::TTL_EXPIRED:   return ICMP_TTL_EXP;
                        case ICMPHeader::SOURCE_QUENCH: return ICMP_SOURCE_QU;
						case ICMPHeader::REDIRECT:	    return ICMP_REDIRECT;
                    }
                }
            }
            break;
        }
    }
    else if (responseIP->protocol == IPPROTO_TCP)
    {   
        // the response is TCP - is it a response to what we sent.
        TCPHeader* responseTCP = (TCPHeader*)(resp.getBlock() + responseIP->getHeaderSize());

        if (sent.ip.destIP      == responseIP->sourceIP &&
            sent.ip.sourceIP    == responseIP->destIP &&
            sent.tcp.destPort   == responseTCP->sourcePort &&
            sent.tcp.sourcePort == responseTCP->destPort &&
            (sent.tcp.seqNum+1) == responseTCP->ackNum)
        {
            if (responseTCP->rst)
                return TCP_RST;

            if (responseTCP->ack && responseTCP->syn)
                return TCP_SYNACK;
        }
    }

    return NOT_VALID;
}

u_short ip_cksum(u_short *addr, int len);


void createTCPPacket (TCPIPHeader& h, const net::InetAddress& target, const net::InetAddress& source, int ttl)
{
    struct fakeheader // used to calculate the checksum
    {
        FakeIPHeader fip;
        TCPHeader tcp;
    };

    u_long seqNum = (rand() << 15) + rand();
    
    // workout checksum 
    fakeheader fh;
    fh.fip.clear();
    fh.tcp.clear();
    
    fh.fip.length = 20;    
    fh.fip.destIP = target.getIPAddress();
    fh.fip.sourceIP = source.getIPAddress();
    fh.fip.protocol = IPPROTO_TCP;

    fh.tcp.destPort = target.getPortNumber();
    fh.tcp.sourcePort = rand() + 0x2000;
    fh.tcp.syn = 1;
    fh.tcp.seqNum = seqNum;
    fh.tcp.ackNum = 0;
    fh.tcp.dataOffset = 5;
    fh.tcp.window = 0x3fff;

    fh.tcp.checksum = in_cksum((u_short*)&fh, sizeof(fh));

    // create the ip& tcp header to send
  
    h.ip.clear();
    h.tcp = fh.tcp;

    h.ip.length = 5;    
    h.ip.destIP = target.getIPAddress();
    h.ip.sourceIP = source.getIPAddress();
    h.ip.protocol = IPPROTO_TCP;
    h.ip.version = 4;
    h.ip.dontFrag = 1;
    h.ip.moreFrags = 0;
    h.ip.id = (u_short)GetCurrentProcessId();
    h.ip.TTL = ttl;
    h.ip.totLength = sizeof (h);
    h.ip.headerChecksum = 0;
    h.ip.headerChecksum = in_cksum((u_short*)&h.ip, sizeof(h.ip)); 
}


ResponsePacketTypes doTCPPing (neo::RefPtr<IRawPacketInterface> rawInterface, const net::InetAddress& target, const net::InetAddress& source, int ttl, net::InetAddress& respFrom, DWORD& pingTime, DWORD timeout)
{
    TCPIPHeader h;
    createTCPPacket (h, target, source, ttl);

    neo::TimeOut timeOutCounter (timeout); 

    rawInterface->sendPacket ((const char*)&h, sizeof(h));

    // get respose
    neo::MemoryBlock recBuffer(0x10000);
    ResponsePacketTypes respType = NOT_VALID;

    do
    {
        if (rawInterface->recPacket (recBuffer, respFrom, timeOutCounter.getRemainingTime()))
        {
            respType = validateResponse (h, recBuffer);
        }
    }
    while ((NOT_VALID == respType) && !timeOutCounter.hasTimedOut());

	pingTime = timeOutCounter.getElapsedTime();

    return respType;
}



void doTraceTCP (TCPTraceSettings& settings, ITraceOutput& out, TraceTerminator& terminator)
{    
    srand ((unsigned)time(NULL));

    net::InetAddress target (settings.remoteHost.c_str());
    
    if (target.getPortNumber() == 0)
        target.setPortNumber(80);

    if (!settings.portRange)
    {
        settings.startPort = target.getPortNumber();
        settings.endPort   = target.getPortNumber();
    }

	neo::RefPtr<IRawPacketInterface> rawInterface;

	if (settings.useRawSockets)
		rawInterface = new RawSocketPacketInterface();
	else
		rawInterface = new WinpcapPacketInterface();

	if (settings.forceGW != "")
		rawInterface->overrideGateway(net::InetAddress(settings.forceGW.c_str()));

	rawInterface->initialise(target);

	for (DWORD port = settings.startPort; port <= settings.endPort; port++)
    {
        target.setPortNumber ((u_short)port);

        bool traceComplete = false;
        out.startTrace (target, settings.noRDNS, settings.maxHops, false);

        for (DWORD hop = settings.startHop; hop < (settings.startHop + settings.maxHops); hop++)
        {
            out.startHop(hop); 

            bool hopComplete = false;
            for (DWORD m = 0; (m < settings.pingsPerHop) & !hopComplete; m++)
            {
                if (terminator.isTerminated())
                    throw std::string("Terminate Event Occurred.");

                DWORD pingTime;
                net::InetAddress resp;
                ResponsePacketTypes respType = doTCPPing (rawInterface, target, rawInterface->getSourceAddress(), hop, resp, pingTime, settings.maxTimeout);

                if (respType == NOT_VALID)
                {
                    out.pingResultTimeout();
                }
                else if (respType == ICMP_NO_ROUTE)
                {
                    out.pingResultBad (resp, "NO ROUTE");
                    traceComplete = true;
                    hopComplete = true;
                }
				else if (respType == ICMP_REDIRECT)
                {
                    out.pingResultBad (resp, "ICMP REDIRECT recieved");
                    traceComplete = true;
                    hopComplete = true;
                }
                else if (respType == TCP_RST)
                {
                    out.destinationReached (resp, pingTime, false);
                    traceComplete = true;
                    hopComplete = true;
                }
                else if (respType == TCP_SYNACK)
                {
                    out.destinationReached (resp, pingTime, true);
                    traceComplete = true;
                    hopComplete = true;
                }
                else
                {
                    out.pingResultGood (resp, pingTime);
                }

                // wait at least 0.5 second between pings....
                // as sending the SYN packets too fast seems to trigger some form of flood detection on some hosts
                if (!settings.noAntiFlood)
                {
                    if ((pingTime < 500) && !hopComplete)
                    {
                        Sleep (500 - pingTime);
                    }
                }
            }

            if (traceComplete)
                break;

            out.endHop ();
        }
        out.endTrace();
    }
}
