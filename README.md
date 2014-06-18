-------------------------------------------------------------------------------
tracetcp (c) 2005 L.M.Witek

http://tracetcp.sourceforge.net/

Questions? Comments? mailto: codetrifle [at] users.sourceforge.net
-------------------------------------------------------------------------------

Description
===========

tracetcp is a traceroute utility for WIN32 that uses TCP SYN packets rather
than ICMP/UDP packets that the usual implementations use, thus bypassing
gateways that block traditional traceroute packets.

In addition to providing the functionality of the standard traceroute
utility tracetcp allows a trace to be performed over any TCP port. This
allows the discovery of what ports a filewall blocks & and also the presence
of any transparent proxies that have been put in place.

-------------------------------------------------------------------------------

Requirements
============

***NOTE***

You *MUST* install the winpcap library (http://www.winpcap.org) for this 
version to work. tracetcp has been tested with version 3.0 and 3.1 beta 4 
of this library.

Previous versions of tracetcp used raw sockets but Microsoft took out support 
for Raw TCP sockets in XP SP2, and as tracetcp uses this feature it caused 
it to stop working.

There is an article about this here that may be of interest:
http://seclists.org/lists/nmap-hackers/2005/Apr-Jun/0000.html

In order to allow the program to function with XP sp2 it now uses the winpcap 
library to send and receive packets. The downside of using this is that the 
program will not function over dial-up connections. If you need to use the 
program with a dial-up connection then you can revert to the old raw sockets 
interface by using the -R switch (this will not work with XP sp2 of course)

tracetcp has been tested on on Windows 2000 & XP (It also probably runs 
on Windows 2003 but has not been tested on this platform). You must be 
logged on to the System as a user that has administrator rights to use tracetcp.

Note: tracetcp *may* work on Windows NT4, 98 or ME but has not been tested on 
these platforms, and I can offer no support for these platforms.

-------------------------------------------------------------------------------

Software Firewalls
==================

Also, I have noticed that tracetcp does not work with various software 
firewalls installed. 

I have tested tracetcp with Zone Alarm, Kerio, Tiny & SyGate, basically 
tracetcp will not work with any of these running. These firewalls block 
either the outgoing or incoming packets that tracetcp is interested in. 
Theres not a lot I can do about it I'm afraid, as this is what the 
firewalls were designed to do.

It works OK with XP's built-in filewall though. 

-------------------------------------------------------------------------------

Installation
============

You *MUST* install the winpcap library (http://www.winpcap.org) first.

Just copy tracetcp.exe into a directory that is in you system PATH.

-------------------------------------------------------------------------------

Usage
=====

From the command prompt:

tracetcp host [options]
    where host = hostName|ipAddress[:portNumber|serviceName]
    if portNumber or serviceName is not present then port 80 (http) 
    is assumed.

Options:
    -?            Displays help information.

    -c            Select condensed output mode

    -h start_hop  Starts trace at hop specified.

    -m max_hops   Maximum number of hops to reach target.

    -n            No reverse DNS lookups for each node.

    -p num_pings  # of pings per hop (default 3).

    -r p1 p2      Multiple traces from port p1 to p2.

    -t timeout    Wait timeout milliseconds for each reply.

    -v            Displays version information.

    -s p1 p2      Easy port scan mode. gives the same result as
                  setting the following options:
	          -cnr p1 p2 -h 128 -m 1 -p 1

    -F            Disables the Anti-flood timer. Normally tracetcp
                  waits *at least* 0.5 seconds between sending out 
                  each packet, because if the packets are sent too 
                  fast some host seem to detect this as some form of 
                  flood and stop responding for a time. This option 
                  disables the 0.5 second timer, so the traces occur
		  faster.

    -R            Use raw socket interface to send/receive packets
                  this will not work on XP sp2. 
		  (you still need winpcap installed)
		  
    -g address    use the specified host as a a gateway to remote
                  systems rather than the default gateway.


Examples:
    tracetcp www.microsoft.com:80 -m 60
    tracetcp post.sponge.com:smtp
    tracetcp 192.168.0.1 -n -t 500

-------------------------------------------------------------------------------

Compiling the source
====================

tracetcp is built with Visual Studio .NET 2003. It may be possible to use
older versions of Visual Studio to build tracetcp, but I have not tried
this.

If you require to rebuild tracetcp from the source then either download the
source distribution from sourceforge or get the latest files from the CVS
archive. (see the sourceforge documentation for help on getting read only
access to the CVS archive)

Visual Studio project files now included in source archive so you can load 
the project: tracetcp.vcproj and build within the IDE.

There is also a build.bat file include with the source that will create the
executable. To use the build.bat file you may need to edit the sdkvars.bat 
to set the PATH, LIB, & INCLUDE Environment variables to refer to your 
installation of Visual Studio. 

-------------------------------------------------------------------------------

Revision History
================

version 0.99.4 beta 23-05-2005
    *fixed problem with in-accurate hop timing.

version 0.99.3 beta 21-05-2005
    *added winpcap support to bypass Microsofts removal of raw tcp sockets 
     in xp sp2.
    *added -g option to override the default gateway
    *added -R option to revert to using raw sockets

version 0.99.2 beta 23-04-2004
    *Added -s option for "Easy port scan"
    *Added abitity to combine options on the command line
     e.g -cnr 10 30 instead of -c -n - r 10 30
    *Updated readme.txt with more info about software firewalls.
    *Fixed bug in anti-flood timeout code.
    *Added -F option to disable the anti-flood timer.
    *complete ip header construction - not leaving any fields for 
     winsock to fill in now.

version 0.99.1 beta 25-08-2003
    *Added start hop option (-h) and changed help to -?
    *Added port range option -r to allow port scanning
    *Separated Tracing code and results display so that different 
     display formats can be supported.
    *Added Condensed output mode (-c)
    *Added pings per hop option (-p)
    *Fixed a few Problems with the way packets were built
    *Visual Studio project files now included in source archive

version 0.99beta 19-08-2003
    First release on sourceforge.

version 0.90beta 21-07-2003
    Internal test version.

-------------------------------------------------------------------------------



