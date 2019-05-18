#pragma once

#include "coremin.h"

namespace Net
{
	/**
	 * @class SocketStream net/socket_stream.h
	 * 
	 * TCP socket
	 */
	class SocketStream
	{
	protected:
		/// Socket file descriptor
		int32 sockfd;

	public:
		/// Default constructor
		FORCE_INLINE SocketStream()
			: sockfd{-1} {}
		
		/// Move constructor
		FORCE_INLINE SocketStream(SocketStream && other)
			: sockfd{other.sockfd}
		{
			other.sockfd = -1;
		}

		/// Move assignment
		FORCE_INLINE SocketStream & operator=(SocketStream && other)
		{
			// Close active connection
			if (sockfd != -1) ::close(sockfd);

			// Move socket descriptor
			sockfd = other.sockfd;
			other.sockfd = -1;
		}

		/// Destructor
		FORCE_INLINE ~SocketStream()
		{
			if (sockfd != -1) ::close(sockfd);
		}

		/// Returns true if socket is initialized
		FORCE_INLINE bool isInit()
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

		/// Create a new tcp socket
		FORCE_INLINE bool init()
		{
			if (sockfd == -1) sockfd = ::socket(AF_INET, SOCK_STREAM, 0);
			return sockfd != -1;
		}

		/// Reset tcp connection
		FORCE_INLINE bool reset()
		{
			return ::shutdown(sockfd, SHUT_RDWR) == 0;
		}

		/**
		 * Bind the socket to the specified ip
		 * 
		 * @param [in] addr ipv4/ipv6 address
		 * @param [in] port port to bind to (host byte order)
		 * @{
		 */
		template<typename IpType = Ipv4>
		FORCE_INLINE bool bind(const IpType & addr)
		{
			return ::bind(sockfd, &addr.__addr, sizeof(addr.__addr)) == 0;
		}
		FORCE_INLINE bool bind(uint16 port = 0)
		{
			return bind<Ipv4>(Ipv4{AF_INET, htons(port), htonl(INADDR_ANY)});
		}
		/// @}

		/**
		 * Listen on bound port
		 * 
		 * @param [in] backlog max queue length (further requests are discarded)
		 */
		FORCE_INLINE bool listen(int32 backlog = 8)
		{
			return ::listen(sockfd, backlog) == 0;
		}

		/**
		 * Accept a new connection
		 * 
		 * @param [out] client new connection
		 * @param [out] addr connection address
		 * @{
		 */
		template<typename IpType = Ipv4>
		FORCE_INLINE bool accept(SocketStream & client, IpType & addr)
		{
			// Close active connection
			if (UNLIKELY(client.sockfd != -1)) ::close(client.sockfd);

			// Accept new connection
			socklen_t addrLen = sizeof(addr.__addr);
			return (client.sockfd = ::accept(sockfd, &addr.__addr, &addrLen)) != -1;
		}
		FORCE_INLINE bool accept(SocketStream & client)
		{
			Ipv4 addr; return accept<Ipv4>(client, addr);
		}
		/// @}

		/**
		 * Connect to a remote socket
		 * 
		 * @param [in] addr remote ipv4/ipv6 address
		 */
		template<typename IpType = Ipv4>
		FORCE_INLINE bool connect(const IpType & addr)
		{
			return ::connect(sockfd, &addr.__addr, sizeof(addr.__addr)) == 0;
		}

		/**
		 * Receive data over this connection
		 * 
		 * @param [out] buffer receive buffer
		 * @param [out] data receive data
		 * @param [in] len receive buffer length
		 * @param [in] flags @see http://man7.org/linux/man-pages/man2/recvmsg.2.html
		 * @return num of read Bytes or true if data was received
		 * @{
		 */
		FORCE_INLINE int32 read(void * buffer, sizet len, uint32 flags = MSG_WAITALL)
		{
			return ::recv(sockfd, buffer, len, flags);
		}

		template<typename T>
		FORCE_INLINE bool read(T & data)
		{
			return read((void*)&data, (sizet)sizeof(T), MSG_WAITALL) == sizeof(T);
		}

		template<typename T, typename AllocT = MallocAnsi>
		FORCE_INLINE bool readArray(Array<T, AllocT> & arr)
		{
			// Get number of elements
			uint64 len; read(len);
			arr.emplace(len);

			// Read data
			return len && read((void*)*arr, arr.getBytes(), MSG_WAITALL) == arr.getBytes();
		}
		/// @}

		/**
		 * Send data over this connection
		 * 
		 * @param [in] buffer source buffer
		 * @param [in] len source buffer length
		 * @param [in] flags @see http://man7.org/linux/man-pages/man2/send.2.html
		 * @return num of wrote Bytes or true if data was sent
		 * @{
		 */
		FORCE_INLINE int32 write(const void * buffer, sizet len, uint32 flags = 0)
		{
			return ::send(sockfd, buffer, len, flags);
		}

		template<typename T>
		FORCE_INLINE bool write(typename ConstRef<T>::Type data)
		{
			return write((const void*)&data, (sizet)sizeof(T), 0) == sizeof(T);
		}

		template<typename T, typename AllocT = MallocAnsi>
		FORCE_INLINE bool writeArray(const Array<T, AllocT> & arr)
		{
			// Write number of elements
			write<uint64>(arr.getCount());

			// Write data
			const sizet len = arr.getBytes();
			return write((const void*)*arr, arr.getBytes(), 0) == len;
		}
		/// @}
	};
} // namespace Net