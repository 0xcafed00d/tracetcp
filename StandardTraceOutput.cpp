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
#include "StandardTraceOutput.h"

using namespace std;

void StandardTraceOutput::startTrace (net::InetAddress& target, bool noRDNS, DWORD maxHops, bool noPort)
{
    m_noRDNS = noRDNS;
    cout << endl << "Tracing route to " << target.getIPAddressAsString();
    if (!noRDNS)
    {
        try
        {
            string lookedUpName;
            lookedUpName = target.lookupHostName();
            cout << " [" << lookedUpName << "]";
        }
        catch (net::SocketException&)
        {
        }
    }

    if (!noPort)
        cout << " on port " << target.getPortNumber();

    cout << endl << "Over a maximum of " << maxHops << " hops." << endl;
}

void StandardTraceOutput::startHop (int hopNumber)
{
    cout << hopNumber << "\t";
    m_goodPings = 0;
}

void StandardTraceOutput::pingResultGood (net::InetAddress& respFrom, DWORD pingTime)
{
    cout << pingTime << " ms" << "\t";
    m_goodPings++;
    m_lastGoodResp = respFrom;
}

void StandardTraceOutput::pingResultBad (net::InetAddress& respFrom, string message)
{
    cout << "\t" << respFrom.getIPAddressAsString() << " reports: " << message << endl;
}

void StandardTraceOutput::pingResultTimeout ()
{
    cout << "*\t";
}

void StandardTraceOutput::destinationReached (net::InetAddress& respFrom, DWORD pingTime, bool portOpen)
{
    cout << "Destination Reached in " << pingTime << " ms. ";
        
    if (portOpen)
        cout << "Connection established to " << respFrom.getIPAddressAsString();
    else
        cout << "Port closed on " << respFrom.getIPAddressAsString();

    cout << endl;
}

void StandardTraceOutput::endHop ()
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

void StandardTraceOutput::endTrace ()
{
    cout << "Trace Complete." << endl;
}
