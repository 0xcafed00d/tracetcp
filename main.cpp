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
#include <exception>

#include "net\socket.h"
#include "net\InitWinsock.h"
#include "neo\CommandOptionParser.h"

#include "StandardTraceOutput.h"
#include "CondensedTraceOutput.h"
#include "tcptrace.h"

using namespace std;

const char optionCharHelp        = '?';
const char optionCharMaxHops     = 'm';
const char optionCharStartHop    = 'h';
const char optionCharTimeout     = 't';
const char optionCharNoRDNS      = 'n';
const char optionCharVersion     = 'v';
const char optionCharPortRange   = 'r';
const char optionCharPingsPerHop = 'p';
const char optionCharOutputMode  = 'c';
const char optionCharEasyScan    = 's';
const char optionCharNoAntiFlood = 'F';
const char optionCharUseLocalIF  = 'i';
const char optionCharForceGW     = 'g';
const char optionCharUseRawSock  = 'R';

void displayVersion ()
{
    cout << endl << "tracetcp v1.0.2 (c) 2016 L.M.Witek" << endl << "build date: " << __DATE__ << " " << __TIME__ << endl;
    cout << "Homepage: http://SimulatedSimian.github.io/tracetcp" << endl;
    cout << "Issues? visit: https://github.com/SimulatedSimian/tracetcp/issues" << endl << endl;
}

void displayHelp (neo::CommandOptionParser& cp)
{
	displayVersion();

    cout << endl << "Usage:  tracetcp host [options]" << endl;
    cout << "    where host = hostName|ipAddress[:portNumber|serviceName]" << endl;
    cout << "    if portNumber or serviceName is not present then port 80 (http) is assumed." << endl << endl;
    
    cout << "Options:" << endl; 
    cp.displayOptionsHelp(cout);
    cout << endl; 

    cout << "Examples: " << endl;
    cout << "    tracetcp www.microsoft.com:80 -m 60" << endl;
    cout << "    tracetcp post.sponge.com:smtp" << endl;
    cout << "    tracetcp 192.168.0.1 -n -t 500" << endl << endl;
}



bool populateSettings(neo::CommandOptionParser& cp, TCPTraceSettings& settings)
{   
    if(cp.getOption(optionCharHelp).isPresent())
    {
        displayHelp(cp);
        return false;
    }

    if(cp.getOption(optionCharVersion).isPresent())
    {
        displayVersion();
        return false;
    }

    try
    {
        if (cp.getOption(neo::CommandOption::UNNAMED).getNumParams() != 1)
            throw neo::CommandOptionException ("Host name not specified. Use -? option for help.");
        else
            settings.remoteHost = cp.getOption(neo::CommandOption::UNNAMED).getParam(0);

		settings.useRawSockets  = cp.getOption(optionCharUseRawSock).isPresent();
		settings.forceGW        = cp.getOption(optionCharForceGW).getParam(0, "");
		//settings.forceInterface = cp.getOption(optionCharUseLocalIF).getParam(0, "");

        settings.noAntiFlood    = cp.getOption(optionCharNoAntiFlood).isPresent();
        settings.noRDNS         = cp.getOption(optionCharNoRDNS).isPresent();
        settings.maxHops        = cp.getOption(optionCharMaxHops).getParamAsInt(0, 1, 255, 30);
        settings.startHop       = cp.getOption(optionCharStartHop).getParamAsInt(0, 1, 255, 1);
        settings.maxTimeout     = cp.getOption(optionCharTimeout).getParamAsInt(0, 1, 99999, 4000);
        settings.portRange      = cp.getOption(optionCharPortRange).isPresent();
        settings.pingsPerHop    = cp.getOption(optionCharPingsPerHop).getParamAsInt(0, 1, 5, 3);
        if (settings.portRange) 
        {
            settings.startPort  = cp.getOption(optionCharPortRange).getParamAsInt(0, 0, 0xffff, 0);
            settings.endPort    = cp.getOption(optionCharPortRange).getParamAsInt(1, 0, 0xffff, 0);
        }

        // easy scan mode 
        if (cp.getOption(optionCharEasyScan).isPresent())
        {
            settings.noRDNS     = true;
            settings.maxHops    = 1;
            settings.portRange  = true;   
            settings.startPort  = cp.getOption(optionCharEasyScan).getParamAsInt(0, 0, 0xffff, 0);
            settings.endPort    = cp.getOption(optionCharEasyScan).getParamAsInt(1, 0, 0xffff, 0);
            settings.startHop   = 128;
            settings.pingsPerHop = 1;
            cp.getOption(optionCharOutputMode).setPresent();
        }
    }
    catch (neo::CommandOptionException& e)
    {
        cerr << e.getMessage() << endl;
        return false;
    }

    return true;
}


void setupCommandOptions (neo::CommandOptionParser& cp)
{
    cp.addOption(neo::CommandOption (neo::CommandOption::UNNAMED, 0, 1, ""));
    cp.addOption(neo::CommandOption (optionCharHelp,       0, 0, "           Displays help information."));
    cp.addOption(neo::CommandOption (optionCharMaxHops,    1, 1, "max_hops   Maximum number of hops to reach target."));
    cp.addOption(neo::CommandOption (optionCharStartHop,   1, 1, "start_hop  Starts trace at hop specified."));
    cp.addOption(neo::CommandOption (optionCharTimeout,    1, 1, "timeout    Wait timeout milliseconds for each reply."));
    cp.addOption(neo::CommandOption (optionCharNoRDNS,     0, 0, "           No reverse DNS lookups for each node."));
    cp.addOption(neo::CommandOption (optionCharVersion,    0, 0, "           Displays version information."));
    cp.addOption(neo::CommandOption (optionCharPortRange,  2, 2, "p1 p2      Multiple traces from port p1 to p2."));
    cp.addOption(neo::CommandOption (optionCharPingsPerHop,1, 1, "num_pings  # of pings per hop (default 3)."));
    cp.addOption(neo::CommandOption (optionCharOutputMode, 0, 0, "           Select condensed output mode."));
    cp.addOption(neo::CommandOption (optionCharEasyScan ,  2, 2, "p1 p2      Scan ports p1 to p2. Eqiv of: -cnr p1 p2 -h 128 -m 1 -p 1"));
    cp.addOption(neo::CommandOption (optionCharNoAntiFlood,0, 0, "           Disables the Anti-flood timer."));
    //cp.addOption(neo::CommandOption (optionCharUseLocalIF, 1, 1, "address    Send from the specified local interface."));
    cp.addOption(neo::CommandOption (optionCharForceGW,    1, 1, "address    Send to remote host using specified gateway."));
    cp.addOption(neo::CommandOption (optionCharUseRawSock, 0, 0, "           Use Raw Sockets to send packets."));
}

TraceTerminator g_traceTerminator;

BOOL ctrlHandler(DWORD ctrlType) 
{ 
    switch (ctrlType) 
    { 
        case CTRL_C_EVENT: 
        case CTRL_CLOSE_EVENT: 
        case CTRL_BREAK_EVENT: 
        case CTRL_LOGOFF_EVENT: 
        case CTRL_SHUTDOWN_EVENT: 
            g_traceTerminator.setTerminate();
            return TRUE;
        default: 
            return FALSE; 
    } 

    return TRUE;
} 


int main(int argc, char* argv[])
{
	timeBeginPeriod(1);
    SetConsoleCtrlHandler((PHANDLER_ROUTINE) ctrlHandler, TRUE); 

    neo::CommandOptionParser cp;

    try
    {
        setupCommandOptions(cp);
        cp.parse(argc, argv);
    }
    catch (neo::CommandOptionException& e)
    {
        cerr << e.getMessage() << endl;
        return -1;
    }

    TCPTraceSettings settings;

    if (populateSettings(cp, settings))
    {
        try
        {
            net::InitWinsock initWS (2, 2);

            if (cp.getOption(optionCharOutputMode).isPresent())
                doTraceTCP (settings, CondensedTraceOutput(), g_traceTerminator);
            else
                doTraceTCP (settings, StandardTraceOutput(), g_traceTerminator);

        }
		catch (std::exception& e)
		{
			cerr << e.what() << endl;
		}
        catch (std::string& e)
        {
            cerr << endl << e << endl;
        }       
    }

    return 0;
}

