//
// Copyright (C) 2004 Andras Varga
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this program; if not, see <http://www.gnu.org/licenses/>.
//


#ifndef __INET_ADDRESSRESOLVER_H
#define __INET_ADDRESSRESOLVER_H


#include <vector>

#include "INETDefs.h"

#include "Address.h"
#include "Address.h"

// Forward declarations:
class IInterfaceTable;
class InterfaceEntry;
class IIPv4RoutingTable;
class IPv6RoutingTable;
class NotificationBoard;

#define DEFAULT_ADDR_TYPE (ADDR_IPv4 | ADDR_IPv6 | ADDR_MAC | ADDR_MODULEPATH | ADDR_MODULEID)

/**
 * Utility class for finding IPv4 or IPv6 address of a host or router.
 *
 * Syntax variations understood:
 *    - literal IPv4 address: "186.54.66.2"
 *    - literal IPv6 address: "3011:7cd6:750b:5fd6:aba3:c231:e9f9:6a43"
 *    - module name: "server", "subnet.server[3]"
 *    - interface of a host or router: "server%eth0", "subnet.server[3]%eth0"
 *    - IPv4 or IPv6 address of a host or router: "server(ipv4)",
 *      "subnet.server[3](ipv6)"
 *    - IPv4 or IPv6 address of an interface of a host or router:
 *      "server%eth0(ipv4)", "subnet.server[3]%eth0(ipv6)"
 *    - routerId: "router1%routerId", "R1%routerId"
 *    - interface of a host or router toward defined another node: "client1>router"
 */
class INET_API AddressResolver
{
  protected:
    // internal
    virtual bool getIPv4AddressFrom(Address &retAddr, IInterfaceTable *ift, bool netmask);
    // internal
    virtual bool getIPv6AddressFrom(Address &retAddr, IInterfaceTable *ift, bool netmask);
    // internal
    virtual bool getMACAddressFrom(Address &retAddr, IInterfaceTable *ift, bool netmask);
    // internal
    virtual bool getModulePathAddressFrom(Address &retAddr, IInterfaceTable *ift, bool netmask);
    // internal
    virtual bool getModuleIdAddressFrom(Address &retAddr, IInterfaceTable *ift, bool netmask);
    // internal
    virtual bool getInterfaceIPv4Address(Address &ret, InterfaceEntry *ie, bool mask);
    // internal
    virtual bool getInterfaceIPv6Address(Address &ret, InterfaceEntry *ie, bool mask);
    // internal
    virtual bool getInterfaceMACAddress(Address &ret, InterfaceEntry *ie, bool mask);
    // internal
    virtual bool getInterfaceModulePathAddress(Address &ret, InterfaceEntry *ie, bool mask);
    // internal
    virtual bool getInterfaceModuleIdAddress(Address &ret, InterfaceEntry *ie, bool mask);

  public:
    enum {
        ADDR_IPv4 = 1,
        ADDR_IPv6 = 2,
        ADDR_MAC = 4,
        ADDR_MODULEPATH = 8,
        ADDR_MODULEID = 16,
        ADDR_MASK = 32
    };

  public:
    AddressResolver() { }
    virtual ~AddressResolver() { }

    /**
     * Accepts dotted decimal notation ("127.0.0.1"), module name of the host
     * or router ("host[2]"), and empty string (""). For the latter, it returns
     * the null address. If module name is specified, the module will be
     * looked up using <tt>simulation.getModuleByPath()</tt>, and then
     * addressOf() will be called to determine its IP address.
     */
    virtual Address resolve(const char *str, int addrType = DEFAULT_ADDR_TYPE);

    /**
     * Utility function: Calls resolve() for each item in the string vector, and
     * returns the result in an address vector. The string vector may come e.g.
     * from cStringTokenizer::asVector().
     */
    virtual std::vector<Address> resolve(std::vector<std::string> strs, int addrType = DEFAULT_ADDR_TYPE);

    /**
     * Similar to resolve(), but returns false (instead of throwing an error)
     * if the address cannot be resolved because the given host (or interface)
     * doesn't have an address assigned yet. (It still throws an error
     * on any other error condition).
     */
    virtual bool tryResolve(const char *str, Address& result, int addrType = DEFAULT_ADDR_TYPE);

    /** @name Utility functions supporting resolve() */
    //@{
    /**
     * Returns IPv4 or IPv6 address of the given host or router.
     *
     * This function uses routingTableOf() to find the IIPv4RoutingTable module,
     * then invokes getAddressFrom() to extract the IP address.
     */
    virtual Address addressOf(cModule *host, int addrType = DEFAULT_ADDR_TYPE);

    /**
     * Similar to addressOf(), but only looks at the given interface
     */
    virtual Address addressOf(cModule *host, const char *ifname, int addrType = DEFAULT_ADDR_TYPE);

    /**
     * Returns IPv4 or IPv6 address of the given host or router.
     *
     * This function find an interface of host connected to destmod
     * then invokes getAddressFrom() to extract the IP address.
     */
    virtual Address addressOf(cModule *host, cModule *destmod, int addrType = DEFAULT_ADDR_TYPE);

    /**
     * Returns the router Id of the given router. Router Id is obtained from
     * the getRouterId() method of the IIPv4RoutingTable submodule.
     */
    virtual Address routerIdOf(cModule *host);

    /**
     * Returns the IPv4 or IPv6 address of the given host or router, given its IInterfaceTable
     * module. For IPv4, the first usable interface address is chosen.
     */
    virtual Address getAddressFrom(IInterfaceTable *ift, int addrType = DEFAULT_ADDR_TYPE);

    /**
     * Returns the IPv4 or IPv6 address of the given interface (of a host or router).
     */
    virtual Address getAddressFrom(InterfaceEntry *ie, int addrType = DEFAULT_ADDR_TYPE);

    /**
     * The function tries to look up the IInterfaceTable module as submodule
     * <tt>"interfaceTable"</tt> or <tt>"networkLayer.interfaceTable"</tt> within
     * the host/router module. Throws an error if not found.
     */
    virtual IInterfaceTable *interfaceTableOf(cModule *host);

    /**
     * The function tries to look up the IIPv4RoutingTable module as submodule
     * <tt>"routingTable"</tt> or <tt>"networkLayer.routingTable"</tt> within
     * the host/router module. Throws an error if not found.
     */
    virtual IIPv4RoutingTable *routingTableOf(cModule *host);

    /**
     * The function tries to look up the IPv6RoutingTable module as submodule
     * <tt>"routingTable6"</tt> or <tt>"networkLayer.routingTable6"</tt> within
     * the host/router module. Throws an error if not found.
     */
    virtual IPv6RoutingTable *routingTable6Of(cModule *host);

    /**
     * The function tries to look up the NotificationBoard module as submodule
     * <tt>"notificationBoard"</tt> within the host/router module. Throws an error
     * if not found.
     */
    virtual NotificationBoard *notificationBoardOf(cModule *host);

    /**
     * Like interfaceTableOf(), but doesn't throw error if not found.
     */
    virtual IInterfaceTable *findInterfaceTableOf(cModule *host);

    /**
     * Like routingTableOf(), but doesn't throw error if not found.
     */
    virtual IIPv4RoutingTable *findRoutingTableOf(cModule *host);

    /**
     * Like interfaceTableOf(), but doesn't throw error if not found.
     */
    virtual IPv6RoutingTable *findRoutingTable6Of(cModule *host);

    /**
     * Like notificationBoardOf(), but doesn't throw error if not found.
     */
    virtual NotificationBoard *findNotificationBoardOf(cModule *host);
    //@}
};


#endif

