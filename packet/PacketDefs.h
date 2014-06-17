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

#pragma once

#include <string>
#include "iputils.h"

#pragma pack(1)

namespace packet
{

    template <class struct_type> void clear_struct (struct_type& strct)
    {
        memset (&strct, 0, sizeof(struct_type));
    }


    struct EthernetAddress
    {
        n_int<u_char> addr[6];

        void setToBroadcast ()
        {
            memset (addr, 0xff, 6);
        }

        std::string toString ()
        {
            char buffer[32];
            sprintf (buffer, "%02x:%02x:%02x:%02x:%02x:%02x", addr[0].get(),addr[1].get(),addr[2].get(),addr[3].get(),addr[4].get(),addr[5].get());
            return buffer;
        }

		bool operator== (EthernetAddress& that)
		{
			return memcmp(addr, that.addr, 6) == 0;
		}

    };

    #define ETHERNET_TYPE_IP  0x800
    #define ETHERNET_TYPE_ARP 0x806

    struct EthernetHeader
    {
	    EthernetAddress destAddr;
	    EthernetAddress srcAddr;
	    n_int<u_short> type;

	    void clear ()
        {
            clear_struct (*this);
        }
    };


    struct ARPPacket
    {
	    n_int<u_short>  hardType;
	    n_int<u_short>  protType;
	    n_int<u_char>   hardSize;
	    n_int<u_char>   protSize;
	    n_int<u_short>  op;
	    EthernetAddress senderEtherAddr;
	    n_int<u_long>   senderIPAddr;
	    EthernetAddress targetEtherAddr;
	    n_int<u_long>   targetIPAddr;

	    void clear ()
        {
            clear_struct (*this);
        }
    };

    struct ARPRequest
    {
        EthernetHeader etherHeader;
        ARPPacket arp;
    };

    /*
    False IP header for checksum calculations
        +--------+--------+--------+--------+
        |           Source Address          |
        +--------+--------+--------+--------+
        |         Destination Address       |
        +--------+--------+--------+--------+
        |  zero  |  PTCL  |    TCP Length   |
        +--------+--------+--------+--------+
    */

    struct FakeIPHeader //  for checksum calculations
    {
        n_int<u_long>  sourceIP;
        n_int<u_long>  destIP;
        n_int<u_char>  zero;
        n_int<u_char>  protocol;
        n_int<u_short> length;

        void clear ()
        {
            clear_struct (*this);
        }
    };


    /*
    IP Header format
        0                   1                   2                   3   
        0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    |Version|  IHL  |Type of Service|          Total Length         |
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    |         Identification        |Flags|      Fragment Offset    |
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    |  Time to Live |    Protocol   |         Header Checksum       |
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    |                       Source Address                          |
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    |                    Destination Address                        |
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    |                    Options                    |    Padding    |
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    */

    struct IPHeader
    {
        union
        {
            n_int<u_char, 0, 4>  length;
            n_int<u_char, 4, 4>  version;
        };
        n_int<u_char>  TOS;
        n_int<u_short> totLength;
        n_int<u_short> id;
        union
        {
            n_int<u_short, 15, 1> unsused;  
            n_int<u_short, 14, 1> dontFrag;  
            n_int<u_short, 13, 1> moreFrags;  
            n_int<u_short, 0, 13> fragOffset;  
        };
        n_int<u_char>  TTL;
        n_int<u_char>  protocol;
        n_int<u_short> headerChecksum;
        n_int<u_long>  sourceIP;
        n_int<u_long>  destIP;

        size_t getHeaderSize()
        {
            return size_t(length) * 4;   
        }
        
        void clear ()
        {
            clear_struct (*this);
        }
    };

    /*
    TCP Header Format 
        0                   1                   2                   3   
        0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    |          Source Port          |       Destination Port        |
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    |                        Sequence Number                        |
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    |                    Acknowledgment Number                      |
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    |  Data |           |U|A|P|R|S|F|                               |
    | Offset| Reserved  |R|C|S|S|Y|I|            Window             |
    |       |           |G|K|H|T|N|N|                               |
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    |           Checksum            |         Urgent Pointer        |
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    |                    Options                    |    Padding    |
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    |                             data                              |
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    */

    struct TCPHeader
    {
        n_int<u_short> sourcePort;
        n_int<u_short> destPort;
        n_int<u_long>  seqNum;
        n_int<u_long>  ackNum;
        union
        {
            n_int<u_char, 4, 4>  dataOffset;
            n_int<u_char, 0, 4>  reserved1;
        };
        union
        {
            n_int<u_char, 0, 1> fin;
            n_int<u_char, 1, 1> syn;
            n_int<u_char, 2, 1> rst;
            n_int<u_char, 3, 1> psh;
            n_int<u_char, 4, 1> ack;
            n_int<u_char, 5, 1> urg;
            n_int<u_char, 6, 2> reserved2;
        };
        n_int<u_short> window;
        n_int<u_short> checksum;
        n_int<u_short> windowPtr;

        void clear ()
        {
            clear_struct (*this);
        }
    };


    // header with ip & tcp headers in it
    struct TCPIPHeader 
    {
        IPHeader ip;
        TCPHeader tcp;

        void clear ()
        {
            ip.clear();
            tcp.clear();
        }
    };


    struct UDPHeader
    {
        n_int<u_short> sourcePort;
        n_int<u_short> destPort;
        n_int<u_short> length;
        n_int<u_short> checksum;
    };


    struct ICMPHeader
    {
        // ICMP Types

        static const int ECHO_REP = 0;
        static const int ECHO_REQ = 8;
        static const int DEST_UNREACH = 3;
        static const int TTL_EXPIRED = 11;
        static const int SOURCE_QUENCH = 4;
		static const int REDIRECT = 5;

        n_int<u_char>  type;
        n_int<u_char>  code;
        n_int<u_short> checksum;

        void clear ()
        {
            clear_struct (*this);
        }
    };


    struct ICMPEchoHeader : public ICMPHeader
    {
        n_int<u_short> id;
        n_int<u_short> seqNum;

        void clear ()
        {
            clear_struct (*this);
        }

        void init (u_short _id, u_short _seqNum)
        {
            id = _id;
            seqNum = _seqNum;
            type = ECHO_REQ;
        }
    };

    struct ICMPErrorHeader : public ICMPHeader
    {
        n_int<u_short> unused1;
        n_int<u_short> unused2;
        IPHeader ipHeader;

        size_t getHeaderSize()
        {
            return ipHeader.getHeaderSize() + sizeof(ICMPHeader) + sizeof(unused1) + sizeof(unused2);
        }
    };


    struct PingReqPacket
    {
        ICMPEchoHeader icmp;
        u_char data[32];
    };

    struct PingRespPacket
    {
        IPHeader ip;
        PingReqPacket ping;
    };
}

#pragma pack()
