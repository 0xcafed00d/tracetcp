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

#if !defined(___TRACETCP__CONDENSEDTRACEOUTPUT__INCLUDED___)
#define ___TRACETCP__CONDENSEDTRACEOUTPUT__INCLUDED___

#include "ITraceOutput.h"

class CondensedTraceOutput : public ITraceOutput
{
    private:
        int  m_goodPings;
        int  m_totalPings;
        bool m_noRDNS;
        net::InetAddress m_target;    
        net::InetAddress m_lastGoodResp;    

    public:
        void startTrace (net::InetAddress& target, bool noRDNS, DWORD maxHops, bool noPort);
        void startHop (int hopNumber);
        void pingResultGood (net::InetAddress& respFrom, DWORD pingTime);
        void pingResultBad (net::InetAddress& respFrom, std::string message);
        void pingResultTimeout ();
        void destinationReached (net::InetAddress& respFrom, DWORD pingTime, bool portOpen);
        void endHop ();
        void endTrace ();
};

#endif