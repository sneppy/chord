#pragma once

#include "coremin.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <ifaddrs.h>
#include <netdb.h>

namespace Net
{
	union Ipv4
	{
	public:
		/// Wildcard ipv4 address
		static const Ipv4 any;

	public:
		struct
		{
			/// Address family (ipv4 or ipv6)
			uint16 family = AF_INET;

			/// Address port
			uint16 port;

			/// Ipv4
			union
			{
				uint32 host;
				ubyte hostBytes[4];
			};
		};

		// Legacy structure
		sockaddr __addr;

	public:
		/// Get port (host byte order)
		FORCE_INLINE uint16 getPort() const
		{
			return ntohs(port);
		}

		/// Set port (host byte order)
		FORCE_INLINE void setPort(uint16 _port)
		{
			port = htons(_port);
		}
	};

	union Ipv6
	{
	public:
		struct
		{
			/// Address family
			uint16 family = AF_INET6;

			/// Address port
			uint16 port;

			/// Ipv6 network prefix
			union
			{
				uint64 network;
				uint16 networkBytes[4];
			};

			/// Ipv6 host
			union
			{
				uint64 host;
				uint16 hostBytes[4];
			};
		};

		/// Legacy structure
		sockaddr __addr;

	public:
		/// Get port (host byte order)
		FORCE_INLINE uint16 getPort() const
		{
			return ntohs(port);
		}

		/// Set port (host byte order)
		FORCE_INLINE void setPort(uint16 _port)
		{
			port = htons(_port);
		}
	};

	/**
	 * Get ip address from string
	 * 
	 * @param [in] str address string
	 * @param [in] defPort default port if none specified (in host byte order)
	 * @return true if correctly parsed
	 */
	template<typename IpType>
	bool parseIpString(IpType & addr, const char * str);

	/**
	 * Get string from ip
	 * 
	 * @param [in] addr ip address
	 * @return ip string
	 */
	template<typename IpType = Ipv4>
	String getIpString(const IpType & addr);

	/**
	 * Get address of first available interface
	 * 
	 * @param [in] family interface type (AF_INET, AF_INET6)
	 * @param [in] port optional port (host byte order)
	 * @return requested address
	 * @{
	 */
	template<typename IpType = Ipv4>
	bool getInterfaceAddr(IpType & addr);

	template<typename IpType = Ipv4>
	LinkedList<IpType> getInterfaceAddrs(uint16 port = 0);
	/// @}


	/**
	 * Returns host ip address(es)
	 * 
	 * @param [in] hostname hostname to resolve
	 * @param [in] port port (host byte order)
	 * @return first address or list of address
	 * @{
	 */
	template<typename IpType = Ipv4>
	bool getHostAddr(IpType & addr, const char * hostname);

	template<typename IpType = Ipv4>
	LinkedList<IpType> getHostAddrs(const char * hostname, uint16 defPort = 0);
	/// @}
} // namespace Net