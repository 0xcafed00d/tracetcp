# Welcome to tracetcp.

tracetcp is a command line traceroute utility for WIN32 that uses TCP SYN packets rather than ICMP/UDP packets that the usual implementations use, thus bypassing gateways that block traditional traceroute packets.

In addition to providing the functionality of the standard traceroute utility tracetcp allows a trace to be performed over any TCP port. This allows the discovery of what ports a filewall blocks and also the presence of any transparent proxies that have been put in place.

# System Requirements

You **MUST** install the winpcap library (http://www.winpcap.org) for this version to work. tracetcp has been tested with version 3.* and 4.* of this library.

Previous versions of tracetcp used raw sockets but Microsoft took out support for Raw TCP sockets in XP SP2, and as tracetcp uses this feature it caused it to stop working.

There is an article about this here that may be of interest: http://seclists.org/lists/nmap-hackers/2005/Apr-Jun/0000.html

In order to allow the program to function with XP sp2 it now uses the winpcap library to send and receive packets. The downside of using this is that the program will not function over dial-up connections. 

tracetcp has been tested on on Windows XP, Vista & 7. You must be logged on to the System as a user that has administrator rights to use tracetcp.

# Installation

You MUST install the winpcap library (http://www.winpcap.org) first.
Just copy tracetcp.exe into a directory that is in you system PATH.

# Usage

From the command prompt:
```
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
```


# Revision History

### version 0.99.5 beta ??-??-2014
* Upgraded solution to Visual Studio 2014
* Moved Repo to GitHub

### version 0.99.4 beta 23-05-2005
* fixed problem with in-accurate hop timing.

### version 0.99.3 beta 21-05-2005
* added winpcap support to bypass Microsofts removal of raw tcp sockets in xp sp2.
* added -g option to override the default gateway
* added -R option to revert to using raw sockets

### version 0.99.2 beta 23-04-2004
* Added -s option for "Easy port scan"
* Added abitity to combine options on the command line e.g -cnr 10 30 instead of -c -n - r 10 30
* Updated readme.txt with more info about software firewalls.
* Fixed bug in anti-flood timeout code.
* Added -F option to disable the anti-flood timer.
* complete ip header construction - not leaving any fields for winsock to fill in now.

### version 0.99.1 beta 25-08-2003
* Added start hop option (-h) and changed help to -?
* Added port range option -r to allow port scanning
* Separated Tracing code and results display so that different display formats can be supported.
* Added Condensed output mode (-c)
* Added pings per hop option (-p)
* Fixed a few Problems with the way packets were built
* Visual Studio project files now included in source archive

### version 0.99 beta 19-08-2003
* First release on sourceforge.

### version 0.90beta 21-07-2003
* Internal test version.

# Example Traces
## tracetcp vs tracert

Many sites block ICMP pings, which makes the use of the traditional route tracing tools difficult, as can be seen by the following trace, which uses the standard Windows tracert utility:

```
$ tracert www.ebay.co.uk

Tracing route to www.ebay.co.uk [66.135.192.41]
over a maximum of 30 hops:

  1     1 ms     1 ms     1 ms  wintermute [192.168.0.1]
  2    10 ms     9 ms     9 ms  10.78.128.1
  3    10 ms     9 ms    10 ms  gsr01-so.blueyonder.co.uk [62.30.193.33]
  4     9 ms     9 ms    10 ms  172.18.14.45
  5    13 ms    13 ms    13 ms  172.18.14.62
  6    13 ms    13 ms    14 ms  tele2-witt-pos.telewest.net [194.117.136.18]
  7    14 ms    13 ms    14 ms  zcr1-so-5-0-0.Londonlnt.cw.net [166.63.222.37]
  8   168 ms   164 ms   162 ms  dcr2-loopback.SantaClara.cw.net [208.172.146.100]
  9   165 ms   164 ms   163 ms  bhr1-pos-0-0.SantaClarasc8.cw.net [208.172.156.198]
 10   164 ms   163 ms   165 ms  csr1-ve243.SantaClarasc8.cw.net [66.35.194.50]
 11     *        *        *     Request timed out.
 12     *        *        *     Request timed out.
 13  .... continues until maximum number of hops reached.
```

Looking at the above trace it is impossible to determine if an intermediate router (at hop 11) has failed or the probes are being blocked.

By using tracetcp we are able to probe all the way to the host machine. Note, however some routers do not report time-to-live exceeded messages as can be by hop #14. From this trace we can see that our packets are reaching the host machine and that it is accepting connections on the specified port (in this case 80).

```
$ tracetcp www.ebay.co.uk

Tracing route to 66.135.192.41 [www.ebay.co.uk] on port 80
Over a maximum of 30 hops.
1       1 ms    1 ms    2 ms    192.168.0.1     [wintermute]
2       10 ms   9 ms    11 ms   10.78.128.1
3       10 ms   11 ms   8 ms    62.30.193.33    [gsr01-so.blueyonder.co.uk]
4       10 ms   9 ms    10 ms   172.18.14.45
5       14 ms   13 ms   14 ms   172.18.14.62
6       12 ms   13 ms   14 ms   194.117.136.18  [tele2-witt-pos.telewest.net]
7       12 ms   12 ms   14 ms   166.63.222.37   [zcr1-so-5-0-0.Londonlnt.cw.net]
8       182 ms  164 ms  164 ms  208.172.146.100 [dcr2-loopback.SantaClara.cw.net]
9       163 ms  163 ms  164 ms  208.172.156.198 [bhr1-pos-0-0.SantaClarasc8.cw.net]
10      165 ms  165 ms  167 ms  66.35.194.50    [csr1-ve243.SantaClarasc8.cw.net]
11      165 ms  165 ms  164 ms  66.35.212.190
12      168 ms  169 ms  169 ms  66.135.207.253
13      166 ms  169 ms  171 ms  66.135.207.174
14      *       *       *       Request timed out.
15      Destination Reached in 170 ms. Connection established to 66.135.192.41
Trace Complete.
```

## Detect a transparent proxy

Many ISPs implement a transparent proxy for certain services (mostly http). Usually these proxies are not visible to the user so it is not possible determine if a connection problem is an issue with the remote host or the proxy. Tracing to ftp.cdrom.com on port 21 (ftp) returns a complete route to the server.

```
$ tracetcp ftp.cdrom.com:ftp

Tracing route to 207.250.14.6 on port 21
Over a maximum of 30 hops.
1       2 ms    1 ms    1 ms    192.168.0.1
2       9 ms    10 ms   8 ms    10.152.79.254
3       10 ms   33 ms   11 ms   62.253.122.137  [rdng-t2cam1-b-v102.inet.ntl.com]
4       27 ms   13 ms   10 ms   62.253.121.129  [winn-t2core-b-ge-wan61.inet.ntl.com]
5       12 ms   10 ms   9 ms    62.253.184.117  [win-bb-b-so-320-0.inet.ntl.com]
6       10 ms   11 ms   13 ms   62.253.185.201  [pop-bb-a-so-000-0.inet.ntl.com]
7       11 ms   9 ms    10 ms   212.113.14.129  [pos7-0.hsipaccess2.london1.level3.net]
8       11 ms   12 ms   11 ms   212.187.131.125 [so-4-0-0.mp2.london1.level3.net]
9       78 ms   76 ms   78 ms   212.187.128.153 [so-1-0-0.bbr2.newyork1.level3.net]
10      76 ms   75 ms   77 ms   64.159.17.104   
11      85 ms   78 ms   93 ms   64.152.40.2     [us-nyc-c-r2-pos6-0.core.viatel.net]
12      78 ms   78 ms   81 ms   66.192.240.33
13      99 ms   97 ms   100 ms  168.215.53.41   [core-02-so-2-2-0-0.chcg.twtelecom.net]
14      123 ms  119 ms  118 ms  168.215.55.203  [dist-01-so-2-2-0-0.mpls.twtelecom.net]
15      119 ms  119 ms  119 ms  66.192.244.203  [hagg-01-ge-0-3-0-0.mpls.twtelecom.net]
16      119 ms  120 ms  120 ms  207.250.14.130
17      Destination Reached in 128 ms. Connection established to 207.250.14.6
Trace Complete.
```

Performing the same trace on port 80 (http) shows the the packets take quite a different route after hop 2 and arrive at the proxy at hop 5. Note that the proxy responds to with the same IP address as the requested host.

```
$ tracetcp ftp.cdrom.com:http

Tracing route to 207.250.14.6 on port 80
Over a maximum of 30 hops.
1       1 ms    1 ms    1 ms    192.168.0.1
2       11 ms   11 ms   10 ms   10.152.79.254
3       10 ms   10 ms   9 ms    62.253.122.9   [rdng-t2cam1-a-v102.inet.ntl.com]
4       11 ms   11 ms   10 ms   62.253.121.1   [winn-t2core-a-ge-wan61.inet.ntl.com]
5       Destination Reached in 10 ms. Connection established to 207.250.14.6
Trace Complete.
```

## Detect a blocked port

Here is a standard trace on port 80:

```
$ tracetcp www.ebay.co.uk

Tracing route to 66.135.192.41 [www.ebay.co.uk] on port 80
Over a maximum of 30 hops.
1       1 ms    1 ms    2 ms    192.168.0.1     [wintermute]
2       10 ms   9 ms    11 ms   10.78.128.1
3       10 ms   11 ms   8 ms    62.30.193.33    [gsr01-so.blueyonder.co.uk]
4       10 ms   9 ms    10 ms   172.18.14.45
5       14 ms   13 ms   14 ms   172.18.14.62
6       12 ms   13 ms   14 ms   194.117.136.18  [tele2-witt-pos.telewest.net]
7       12 ms   12 ms   14 ms   166.63.222.37   [zcr1-so-5-0-0.Londonlnt.cw.net]
8       182 ms  164 ms  164 ms  208.172.146.100 [dcr2-loopback.SantaClara.cw.net]
9       163 ms  163 ms  164 ms  208.172.156.198 [bhr1-pos-0-0.SantaClarasc8.cw.net]
10      165 ms  165 ms  167 ms  66.35.194.50    [csr1-ve243.SantaClarasc8.cw.net]
11      165 ms  165 ms  164 ms  66.35.212.190
12      168 ms  169 ms  169 ms  66.135.207.253
13      166 ms  169 ms  171 ms  66.135.207.174
14      *       *       *       Request timed out.
15      Destination Reached in 170 ms. Connection established to 66.135.192.41
Trace Complete.
```

If we do the same trace but this time we use port 135 we can see that it is blocked after hop 2. This block was put in place by my ISP to try to limit the damage being caused by a worm, that spread by exploiting a vulnerability in DCOM.

```
$ tracetcp www.ebay.co.uk:135

Tracing route to 66.135.192.41 [www.ebay.co.uk] on port 135
Over a maximum of 30 hops.
1       1 ms    1 ms    1 ms    192.168.0.1     [wintermute]
2       10 ms   13 ms   9 ms    10.78.128.1
3       *       *       *       Request timed out.
4       *       *       *       Request timed out.
5       *       *       *       Request timed out.
6 ... continues until maximum number of hops reached.
```

## Scan for Blocked Outgoing Ports

While it is useful to see that a port is being blocked:

```
$ tracetcp www.ebay.co.uk:135

Tracing route to 66.135.192.41 [www.ebay.co.uk] on port 135
Over a maximum of 30 hops.
1       1 ms    1 ms    1 ms    192.168.0.1     [wintermute]
2       10 ms   13 ms   9 ms    10.78.128.1
3       *       *       *       Request timed out.
4       *       *       *       Request timed out.
5       *       *       *       Request timed out.
6 ... continues until maximum number of hops reached.
```

tracetcp can be used to scan a range of ports to find which are blocked and which are not. The command line seems a little complex so I'll go though them all. We are scanning the range of ports 130-140 (-r 130 140). To speed things up the timeout is set to 0.5 seconds (-t 500). Condensed output mode is enabled (-c) or we will get screens full of output. Reverse DNS lookups are disabled (-n). Each packet we send has a TTL set to 3 (-h 3), is only sent for 1 hop (-m 1), and only 1 packet is sent per hop (-p).

Note that it doesn't really matter what host is used for the trace as none of the packets will ever reach it as they will all expire before reaching the host.

```
$ tracetcp www.ebay.co.uk -h 3 -m 1 -p 1 -t 500 -c -r 130 140 -n
[66.135.192.41:130]  3  11 ms   62.30.193.33
[66.135.192.41:131]  3  11 ms   62.30.193.33
[66.135.192.41:132]  3  10 ms   62.30.193.33
[66.135.192.41:133]  3  10 ms   62.30.193.33
[66.135.192.41:134]  3  13 ms   62.30.193.33
[66.135.192.41:135]  3  *       Request timed out.
[66.135.192.41:136]  3  10 ms   62.30.193.33
[66.135.192.41:137]  3  *       Request timed out.
[66.135.192.41:138]  3  *       Request timed out.
[66.135.192.41:139]  3  *       Request timed out.
[66.135.192.41:140]  3  11 ms   62.30.193.33
```

By changing the command line slightly we can trace from hop 1 to 3 for each port (-h 1 -m 3) so as to determine at which hop the ports are blocked. Port 135 is block by my ISPs router at hop 2, and ports 137, 138, 139 are blocked by my own router at hop 1.

```
$ tracetcp www.ebay.co.uk -h 1 -m 3 -p 1 -t 500 -c -r 134 140 -n
[66.135.208.41:134]  1  1 ms    192.168.0.1
[66.135.208.41:134]  2  12 ms   10.78.128.1
[66.135.208.41:134]  3  10 ms   62.30.193.33

[66.135.208.41:135]  1  1 ms    192.168.0.1
[66.135.208.41:135]  2  10 ms   10.78.128.1
[66.135.208.41:135]  3  *       Request timed out.

[66.135.208.41:136]  1  1 ms    192.168.0.1
[66.135.208.41:136]  2  11 ms   10.78.128.1
[66.135.208.41:136]  3  9 ms    62.30.193.33

[66.135.208.41:137]  1  1 ms    192.168.0.1
[66.135.208.41:137]  2  *       Request timed out.
[66.135.208.41:137]  3  *       Request timed out.

[66.135.208.41:138]  1  1 ms    192.168.0.1
[66.135.208.41:138]  2  *       Request timed out.
[66.135.208.41:138]  3  *       Request timed out.

[66.135.208.41:139]  1  1 ms    192.168.0.1
[66.135.208.41:139]  2  *       Request timed out.
[66.135.208.41:139]  3  *       Request timed out.

[66.135.208.41:140]  1  1 ms    192.168.0.1
[66.135.208.41:140]  2  12 ms   10.78.128.1
[66.135.208.41:140]  3  187 ms  62.30.193.33
```

## Perform a Port scan

In a similar way to the previous trace (Scanning for blocked outgoing ports), tracetcp can be used to determine what ports are open an a particular host. To port scan the initial TTL of the packets must be greater than the number of hops to the host being scanned so that the connect packets don't expire on the way. Below is a port scan of my router that shows FTP, telnet and HTTP are all open. (the latest version of tracetcp has an option -s that makes scanning easier)

```
$ tracetcp 192.168.0.1 -r 20 80 -c -m 1 -t 100 -h 50
[192.168.0.1:20]  1     *       *       *       Request timed out.
[192.168.0.1:21]  1     Dest. in 1 ms. Port OPEN on 192.168.0.1
[192.168.0.1:22]  1     *       *       *       Request timed out.
[192.168.0.1:23]  1     Dest. in 2 ms. Port OPEN on 192.168.0.1
[192.168.0.1:24]  1     *       *       *       Request timed out.
[192.168.0.1:25]  1     *       *       *       Request timed out.
[192.168.0.1:26]  1     *       *       *       Request timed out.

repeats until.....

[192.168.0.1:77]  1     *       *       *       Request timed out.
[192.168.0.1:78]  1     *       *       *       Request timed out.
[192.168.0.1:79]  1     *       *       *       Request timed out.
[192.168.0.1:80]  1     Dest. in 3 ms. Port OPEN on 192.168.0.1
```
