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

#include "iputils.h"
#include <iphlpapi.h>
#include "PacketException.h"

#pragma comment(lib, "iphlpapi.lib")

#include "..\net\Socket.h"

namespace packet
{

    /*
    *          I N _ C K S U M
    *
    * Checksum routine for Internet Protocol family headers (C Version)
    * Taken from Origninal ping source code written by Mike Muuss
    */
    u_short in_cksum(u_short *addr, int len)
    {
        int nleft = len;
        u_short *w = addr;
        u_short answer;
        int sum = 0;

        /*
        *  Our algorithm is simple, using a 32 bit accumulator (sum),
        *  we add sequential 16 bit words to it, and at the end, fold
        *  back all the carry bits from the top 16 bits into the lower
        *  16 bits.
        */
        while( nleft > 1 )  {
            sum += *w++;
            nleft -= 2;
        }

        /* mop up an odd byte, if necessary */
        if( nleft == 1 ) {
            u_short u = 0;

            *(u_char *)(&u) = *(u_char *)w ;
            sum += u;
        }

        /*
        * add back carry outs from top 16 bits to low 16 bits
        */
        sum = (sum >> 16) + (sum & 0xffff); /* add hi 16 to low 16 */
        sum += (sum >> 16);         /* add carry */
        answer = ~sum;              /* truncate to 16 bits */
        return (ntohs(answer));
        //return (answer);
    }

	// for a given destination address - return the local endpoint address and the gateway address that 
    // will be used for a connection to the destination.
    // if the local address and the gateway address are the same then the destination is on a local network.
    void getRouteInfo (net::InetAddress destination, net::InetAddress& localAddr, net::InetAddress& gatewayAddr)
    {
        MIB_IPFORWARDROW routeInfo;
        DWORD res = GetBestRoute(htonl(destination.getIPAddress()), 0, &routeInfo);
        if (res != NO_ERROR)
            throw PacketException ("getRouteInfo: Error");

        gatewayAddr.set(ntohl(routeInfo.dwForwardNextHop), 0);

        destination.setPortNumber(123); 
        net::Socket s(SOCK_DGRAM);
        s.connect(destination);
        localAddr = s.getLocalAddress();
    }
}