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

#include <iostream>
#include "CondensedTraceOutput.h"

using namespace std;

void CondensedTraceOutput::startTrace (net::InetAddress& target, bool noRDNS, DWORD maxHops, bool noPort)
{
    m_target = target;
    m_noRDNS = noRDNS;
}

void CondensedTraceOutput::startHop (int hopNumber)
{
    cout << "["<< m_target.getIPAddressAsString() << ":" << m_target.getPortNumber() << "]  " << hopNumber << "\t";
    m_goodPings = 0;
}

void CondensedTraceOutput::pingResultGood (net::InetAddress& respFrom, DWORD pingTime)
{
    cout << pingTime << " ms" << "\t";
    m_goodPings++;
    m_lastGoodResp = respFrom;
}

void CondensedTraceOutput::pingResultBad (net::InetAddress& respFrom, string message)
{
    cout << "\t" << respFrom.getIPAddressAsString() << " reports: " << message << endl;
}

void CondensedTraceOutput::pingResultTimeout ()
{
    cout << "*\t";
}

void CondensedTraceOutput::destinationReached (net::InetAddress& respFrom, DWORD pingTime, bool portOpen)
{
    cout << "Dest. in " << pingTime << " ms. ";
        
    if (portOpen)
        cout << "Port OPEN on " << respFrom.getIPAddressAsString();
    else
        cout << "Port CLOSED on " << respFrom.getIPAddressAsString();

    cout << endl;
}

void CondensedTraceOutput::endHop ()
{
    if (m_goodPings)
    {
        cout << m_lastGoodResp.getIPAddressAsString() << "\t"; 

        if (!m_noRDNS)
        {
            try
            {
                string name = m_lastGoodResp.lookupHostName();
                cout << "[" << name << "]";
            }
            catch (net::SocketException&) {}
        }
        cout << endl;
    }
    else
    {
        cout << "Request timed out." << endl;
    }
}

void CondensedTraceOutput::endTrace ()
{
}
