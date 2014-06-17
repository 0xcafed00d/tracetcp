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

#include <winsock2.h>
#include <stdint.h>

#pragma once

#include "..\net\InetAddress.h"

namespace packet
{

    // n_int<type> network int - repesents the network forms (ie big-endian) forms of specified types,
    // while providing a host-format native interface for reading and writing. saves having to call 
    // ntoh*() and hton*() when reading and writing packet structures. 
    // can also represnt sub-ranges of bits within the value.
    // used to build packet structures. 
    // size_of (n_int<type>) == size_of (type)
    template <typename int_type, int start_bit = 0, int num_bits = (sizeof(int_type)*8)> 
	class n_int 
    {
        private:
            int_type _val;

            unsigned long n_to_h (unsigned long value)
            {
                return ntohl(value);
            }

            unsigned short n_to_h (unsigned short value)
            {
                return ntohs(value);
            }

            unsigned char n_to_h (unsigned char value)
            {
                return value;
            }

            unsigned long h_to_n (unsigned long value)
            {
                return htonl(value);
            }

            unsigned short h_to_n (unsigned short value)
            {
                return htons(value);
            }

            unsigned char h_to_n (unsigned char value)
            {
                return value;
            }

            int_type generateMask ()
            {
                int_type mask = ~int_type(0);
				mask = ~(int_type)((uint64_t)mask << num_bits);
                mask = mask << start_bit;
                return mask;
            }

        public:
            int_type get()
            {
                if (start_bit == 0 && num_bits == (sizeof(int_type)*8))
                {
                    return n_to_h(_val);
                }
                else
                {          
                    return (n_to_h(_val) & generateMask()) >> start_bit;
                }
            }

            void set(int_type val)
            {
                if (start_bit == 0 && num_bits == (sizeof(int_type)*8))
                {
                    _val = h_to_n(val);
                }
                else
                {
                    int_type mask = generateMask();
                    val = (val << start_bit) & mask;
                    val = (n_to_h(_val) & ~mask) | val;
                    _val = h_to_n(val);
                }
            }

            operator int_type ()
            {
                return get ();
            }

            int_type operator = (int_type val)
            {
                set(val);
                return get ();
            }

            int_type getRaw()
            {
                return _val;
            }
    };


    u_short in_cksum(u_short *addr, int len);


    // for a given destination address - return the local endpoint address and the gateway address that 
    // will be used for a connection to the destination.
    // if the local address and the gateway address are the same then the destination is on a local network.
    void getRouteInfo (net::InetAddress destination, net::InetAddress& localAddr, net::InetAddress& gatewayAddr);
}