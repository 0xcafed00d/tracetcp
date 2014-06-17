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

#include <winsock2.h>

#include "PacketInterface.h"
#include "PacketDefs.h"

#include "..\net\InetAddress.h"

namespace packet
{
    class ARP
    {   
        private:
            PacketInterface& m_packetInterface;
    
        public:
            ARP(PacketInterface& packetInterface);
            ~ARP();

            void whoHas (net::InetAddress srcAddr, net::InetAddress queryAddr, EthernetAddress& mac);
    };
}