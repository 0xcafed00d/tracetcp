
#include <iostream>
using namespace std;

#include "net\socket.h"
#include "net\InitWinsock.h"
#include "neo\CommandOptionParser.h"

#include "packet\InterfaceEnumerator.h"
#include "packet\PacketException.h"
#include "packet\PacketInterface.h"
#include "packet\iputils.h"
#include "packet\ARP.h"

/*

sendPacket (remoteIP)
{
    localIP = local interface to use to send to remote host
    if (remoteIP and localIP are on same subnet)
    {
        remoteMAC = ARP.whoHas (remoteIP)
        sendPacket to remoteMac
    }
    else
    {
        gatewayIP = getGateway (localIP)
        gatewayMAC = ARP.whoHas (gatewayIP)
        sendPacket to gatewayMac
    }
}

*/



int mainx(int argc, char* argv[])
{
    net::InitWinsock initws (2,2);

    packet::InterfaceEnumerator ie;

    try
    {
        net::InetAddress gateway;
        net::InetAddress local;
        net::InetAddress remote(argv[1]);

        packet::getRouteInfo(remote, local, gateway);
        cout << "remote: " << remote << " local: " << local << " gateway: " << gateway << endl;

        ie.findLocalInterfaces();
        for (size_t n = 0; n < ie.getInterfaceCount(); n++)
        {
            const pcap_if_t& iface = ie.getInterface(n);
            cout << iface.description << endl;

            packet::ListAccessor<pcap_addr> addresses (iface.addresses);
            for (size_t addrIndex = 0; addrIndex < addresses.getListLength(); addrIndex++)
            {
                pcap_addr* address = addresses.at(addrIndex);
                net::InetAddress inetAddr(address->addr);
                cout << inetAddr.getIPAddressAsString() << endl;
            }
        }

		const pcap_if_t& iface = ie.getInterface(1);
        packet::ListAccessor<pcap_addr> addresses (iface.addresses);
        net::InetAddress localAddress(addresses.at(0)->addr);
        
        packet::PacketInterface pac;
		pac.openLocal(iface.name, 100, 50);
        packet::ARP arp (pac);

        net::InetAddress addr ("www.google.com");
        packet::EthernetAddress macAddr;

        arp.whoHas (localAddress, addr, macAddr);
        
        cout << macAddr.toString() << endl;

    }
    catch (packet::PacketException& e)
    {
        cout << e.getMessage() << endl;
    }
		return 0;
}