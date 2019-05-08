#include "net/types.h"

namespace Net
{
	//////////////////////////////////////////////////
	// Ipv4
	//////////////////////////////////////////////////

	const Ipv4 Ipv4::any = Ipv4{AF_INET, 0x0000, 0x00000000};

	//////////////////////////////////////////////////
	// Scope functions
	//////////////////////////////////////////////////

	template<>
	bool parseIpString(Ipv4 & addr, const char * str)
	{
		uint32 n = 0;
		uint32 bit = 0;
		uint32 byte = 0;
		ubyte _ = 1;
		const char * cc = str;

		// Get ip
		for (char c = *cc; _; c = *++cc)
		{
			switch (c)
			{
				case ':':
					// Break and read port
					_ = 0;
				case '.':
					// Add component to out ip
					addr.hostBytes[byte++] = n; n = 0;
					//addr.host |= n << bit; n = 0;

					// Next byte
					bit += 8U;
					break;

				case '\0':
				case '\n':
				case '\r':
					// Add component
					addr.hostBytes[byte++] = n; n = 0;
					//addr.host |= n << bit; n = 0;

					// No port, return
					// If we read at least one character, parsing is successful
					return cc != str;
			
				default:
					// Must be a number
					if (c < '0' || c > '9')
						return false;

					n *= 10;
					n += c - '0';
					break;
			}
		}

		// Get port
		for (char c = *cc; c >= '0' && c <= '9'; c = *++cc)
			n *= 10, n += *cc - '0';
		addr.port = htons(n);

		// Parsing OK
		return true;
	}

	template<>
	String getIpString(const Ipv4 & addr)
	{
		char str[INET_ADDRSTRLEN + 1 + 6];

		// Ip
		inet_ntop(AF_INET, &addr.host, str, INET_ADDRSTRLEN);
		// Port
		snprintf(str + 17, 6, "%u", htons(addr.port));

		return String(str) + ":" + String(str + 17);
	}

	template<>
	bool getInterfaceAddr(Ipv4 & addr)
	{
		// Stash port (network byte order)
		uint16 port = addr.port;

		ifaddrs * ifs;
		if (getifaddrs(&ifs) == 0)
		{
			for (auto it = ifs; it != nullptr; it = it->ifa_next)
			{
				if (it->ifa_addr && it->ifa_addr->sa_family == AF_INET && !(it->ifa_flags & IFF_LOOPBACK))
				{
					addr.__addr = *it->ifa_addr;
					addr.port = port;

					freeifaddrs(ifs);
					return true;
				}
			}
			
			// Dealloc list
			freeifaddrs(ifs);
		}

		return false;
	}

	template<>
	bool getHostAddr(Ipv4 & addr, const char * hostname)
	{
		// Stash port (network byte order)
		uint16 port = addr.port;

		// Copy hostname
		String hn(hostname);

		// Parse port (if any)
		{
			char * it = *hn, * const end = *hn + hn.getLength();
			for (char c = *it; c != ':' && it != end; c = *++it);

			if (it != end)
			{
				// Reset default port
				port = 0;

				// Terminate here, otherwise getaddrinfo doesn't work
				*it = '\0';

				// Parse port
				for (char c = *++it; it != end; c = *++it)
					if (c >= '0' && c <= '9') port *= 10, port += c - '0';
				port = htons(port);
			}
		}

		addrinfo * ais;
		addrinfo * hint = new addrinfo; hint->ai_family = AF_INET;
		if (getaddrinfo(*hn, nullptr, hint, &ais) == 0)
		{
			for (auto it = ais; it != nullptr; it = it->ai_next)
			{
				if (it->ai_addr)
				{
					addr.__addr = *it->ai_addr;
					addr.port = port;

					delete hint;
					freeaddrinfo(ais);
					return true;
				}
			}

			// Free list
			freeaddrinfo(ais);
		}

		delete hint;
		return false;
	}

	template<>
	LinkedList<Ipv4> getHostAddrs(const char * hostname, uint16 defPort)
	{
		// TODO: handle port

		LinkedList<Ipv4> out;
		addrinfo * ais;
		addrinfo * hint = new addrinfo; hint->ai_family = AF_INET;
		if (getaddrinfo(hostname, nullptr, hint, &ais) == 0)
		{
			for (auto it = ais; it != nullptr; it = it->ai_next)
			{
				if (it->ai_addr)
				{
					Ipv4 addr = Ipv4{__addr : *it->ai_addr};
					addr.setPort(defPort);

					out.push(addr);
				}
			}

			// Free list
			freeaddrinfo(ais);
		}

		delete hint;
		return out;
	}
} // namespace Net