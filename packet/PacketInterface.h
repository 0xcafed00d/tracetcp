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

#pragma once

#include <pcap.h>
#include <string>

#include "PacketDefs.h"
#include "PacketException.h"

namespace packet
{
	class PacketInterface
	{
		private:
			pcap_t* m_interfaceHandle;
			char m_errorBuffer[PCAP_ERRBUF_SIZE];
            EthernetAddress m_macAddr;

		public:
			PacketInterface();
			~PacketInterface();

			void openLocal (std::string name, int snaplen, int timeout);
			void close ();

			void sendPacket (char* packet, int size);
            //returns false if timeout occurs
            bool readPacket (pcap_pkthdr **header, const u_char **pkt_data);

            static void getMACAddress (std::string name, EthernetAddress* MACAddr);
   			void getMACAddress (EthernetAddress* MACAddr);
	};
}
