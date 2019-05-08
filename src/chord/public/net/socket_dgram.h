#pragma once

#include "coremin.h"
#include "types.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

namespace Net
{
	/**
	 * @class SocketDgram socket_dgram.h
	 * 
	 * UDP socket
	 */
	class SocketDgram
	{
	protected:
		/// Socket file descriptor
		int32 sockfd;

	public:
		/// Default constructor
		FORCE_INLINE SocketDgram()
			: sockfd(-1) {}

		/// Destructor
		FORCE_INLINE ~SocketDgram()
		{
			// Close socket
			if (sockfd != -1) ::close(sockfd);
		}

		/// Returns true if it's valid
		FORCE_INLINE bool isInit() const
		{
			return sockfd != -1;
		}

		/// Get socket binding address
		/// @{
		template<typename IpType = Ipv4>
		FORCE_INLINE void getAddress(IpType & addr) const
		{
			uint32 addrLen = sizeof(addr.__addr);
			::getsockname(sockfd, &addr.__addr, &addrLen);
		}

		template<typename IpType = Ipv4>
		FORCE_INLINE IpType getAddress() const
		{
			// Get socket address
			Ipv4 out; getAddress(out);
			return out;
		}
		/// @}

		/// Initialize socket
		FORCE_INLINE bool init()
		{
			if (sockfd < 0) sockfd = ::socket(AF_INET, SOCK_DGRAM, 0);

			// Return true if socket is initialized
			return sockfd >= 0;
		}

		/**
		 * Bind socket to the provided address
		 * if address is provided, the address
		 * is modified with the actual address
		 * port and ip
		 * 
		 * If no parameter is provided, let
		 * system assign a free port on the
		 * local loopback (ipv4)
		 * 
		 * @param [in] addr address to bind to
		 * @return operation status
		 */
		template<typename IpType = Ipv4>
		FORCE_INLINE bool bind(IpType & addr)
		{
			if (::bind(sockfd, &addr.__addr, sizeof(addr.__addr)) < 0)
			{
				fprintf(stderr, "%s\n", strerror(errno));
				return false;
			}

			// Get actual address
			getAddress(addr);

			return true;
		}
		FORCE_INLINE bool bind()
		{
			Ipv4 addr = Ipv4::any;
			return bind<Ipv4>(addr);
		}
		/// @}

		/**
		 * Receive data
		 * 
		 * @param [out] buffer buffer allocated for data
		 * @param [in] len buffer length in bytes
		 * @param [out] val out value
		 * @param [out] sender sender ipv4 address
		 * @return num bytes read or status
		 * @{
		 */
		template<typename IpType = Ipv4>
		FORCE_INLINE int32 read(void * buffer, sizet len, IpType & sender)
		{
			socklen_t addrLen = sizeof(sender.__addr);
			return ::recvfrom(sockfd, buffer, len, 0, &sender.__addr, &addrLen);
		}
		FORCE_INLINE int32 read(void * buffer, sizet len)
		{
			Ipv4 _;
			return read(buffer, len, _);
		}

		template<typename T, typename IpType = Ipv4>
		FORCE_INLINE bool read(T & val, IpType & sender)
		{
			return read((void*)&val, (sizet)sizeof(T), sender) == sizeof(T);
		}
		template<typename T>
		FORCE_INLINE bool read(T & val)
		{
			Ipv4 _;
			return read(val, _);
		}
		/// @}

		/**
		 * Write data
		 * 
		 * @param [in] buffer buffer with data
		 * @param [in] len buffer length in bytes
		 * @param [in] val value to write
		 * @param [in] recipient recipient ipv4 address
		 * @return num bytes written or status
		 * @{
		 */
		template<typename IpType = Ipv4>
		FORCE_INLINE int32 write(const void * buffer, sizet len, const IpType & recipient)
		{
			return ::sendto(sockfd, buffer, len, 0, &recipient.__addr, sizeof(recipient.__addr));
		}

		template<typename T, typename IpType = Ipv4>
		FORCE_INLINE bool write(typename ConstRef<T>::Type val, const IpType & recipient)
		{
			return write((const void*)&val, (sizet)sizeof(T), recipient) == sizeof(T);
		}
		/// @}
	};
} // namespace Net