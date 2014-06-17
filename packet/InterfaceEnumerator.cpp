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

#include "InterfaceEnumerator.h"
#include "PacketException.h"

#pragma comment(lib, "wpcap.lib")


namespace packet
{
    InterfaceEnumerator::InterfaceEnumerator () : m_interfaces(NULL), m_interfaceIterator(NULL)
    {
    }

    InterfaceEnumerator::~InterfaceEnumerator ()
    {
        cleanUp();
    }

    void InterfaceEnumerator::cleanUp ()
    {
        if (m_interfaces)
            pcap_freealldevs(m_interfaces);
        m_interfaces = NULL;
        m_interfaceIterator.setList(NULL);
    }

    void InterfaceEnumerator::findLocalInterfaces ()
    {
        cleanUp();
        if (pcap_findalldevs(&m_interfaces, m_errorBuffer) == -1)
        {
            throw PacketException (m_errorBuffer);
        }
  
        m_interfaceIterator.setList(m_interfaces);
    }

    size_t InterfaceEnumerator::getInterfaceCount ()
    {
        return m_interfaceIterator.getListLength();
    }

    const pcap_if_t& InterfaceEnumerator::getInterface (size_t interfaceIndex)
    {
        if (interfaceIndex < 0 || interfaceIndex >= getInterfaceCount())
            throw PacketException ("InterfaceEnumerator::getInterface index out of range");

        return *m_interfaceIterator.at (interfaceIndex);
    }
}