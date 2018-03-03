/**
 * WinPR: Windows Portable Runtime
 * Windows Sockets (Winsock)
 *
 * Copyright 2012 Marc-Andre Moreau <marcandre.moreau@gmail.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <uzi/crt.h>
#include <uzi/synch.h>

#include <uzi/winsock.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_SYS_FILIO_H
#include <sys/filio.h>
#endif
#ifdef HAVE_SYS_SOCKIO_H
#include <sys/sockio.h>
#endif

#ifndef _WIN32
#include <fcntl.h>
#endif

#ifdef __APPLE__
#define WSAIOCTL_IFADDRS
#include <ifaddrs.h>
#endif

#ifdef _WIN32

#if (_WIN32_WINNT < 0x0600)

PCSTR winpr_inet_ntop(INT Family, PVOID pAddr, PSTR pStringBuf, size_t StringBufSize)
{
	if (Family == AF_INET)
	{
		struct sockaddr_in in;

		memset(&in, 0, sizeof(in));
		in.sin_family = AF_INET;
		memcpy(&in.sin_addr, pAddr, sizeof(struct in_addr));
		getnameinfo((struct sockaddr*) &in, sizeof(struct sockaddr_in), pStringBuf, StringBufSize, NULL, 0, NI_NUMERICHOST);

		return pStringBuf;
	}
	else if (Family == AF_INET6)
	{
		struct sockaddr_in6 in;

		memset(&in, 0, sizeof(in));
		in.sin6_family = AF_INET6;
		memcpy(&in.sin6_addr, pAddr, sizeof(struct in_addr6));
		getnameinfo((struct sockaddr*) &in, sizeof(struct sockaddr_in6), pStringBuf, StringBufSize, NULL, 0, NI_NUMERICHOST);

		return pStringBuf;
	}

	return NULL;
}

INT winpr_inet_pton(INT Family, PCSTR pszAddrString, PVOID pAddrBuf)
{
	SOCKADDR_STORAGE addr;
	int addr_len = sizeof(addr);

	if ((Family != AF_INET) && (Family != AF_INET6))
		return -1;

	if (WSAStringToAddressA((char*) pszAddrString, Family, NULL, (struct sockaddr*) &addr, &addr_len) != 0)
		return 0;

	if (Family == AF_INET)
	{
		memcpy(pAddrBuf, &((struct sockaddr_in*) &addr)->sin_addr, sizeof(struct in_addr));
	}
	else if (Family == AF_INET6)
	{
		memcpy(pAddrBuf, &((struct sockaddr_in6*) &addr)->sin6_addr, sizeof(struct in6_addr));
	}

	return 1;
}

#endif /* (_WIN32_WINNT < 0x0600) */

#else /* _WIN32 */

#include <netdb.h>
#include <errno.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <net/if.h>

#ifndef MSG_NOSIGNAL
#define MSG_NOSIGNAL 0
#endif

int WSAStartup(WORD wVersionRequired, LPWSADATA lpWSAData)
{
	ZeroMemory(lpWSAData, sizeof(WSADATA));

	lpWSAData->wVersion = wVersionRequired;
	lpWSAData->wHighVersion = MAKEWORD(2, 2);

	return 0; /* success */
}

int WSACleanup(void)
{
	return 0; /* success */
}

void WSASetLastError(int iError)
{
	switch (iError)
	{
		/* Base error codes */

		case WSAEINTR:
			errno = EINTR;
			break;
		case WSAEBADF:
			errno = EBADF;
			break;
		case WSAEACCES:
			errno = EACCES;
			break;
		case WSAEFAULT:
			errno = EFAULT;
			break;
		case WSAEINVAL:
			errno = EINVAL;
			break;
		case WSAEMFILE:
			errno = EMFILE;
			break;

		/* BSD sockets error codes */

		case WSAEWOULDBLOCK:
			errno = EWOULDBLOCK;
			break;
		case WSAEINPROGRESS:
			errno = EINPROGRESS;
			break;
		case WSAEALREADY:
			errno = EALREADY;
			break;
		case WSAENOTSOCK:
			errno = ENOTSOCK;
			break;
		case WSAEDESTADDRREQ:
			errno = EDESTADDRREQ;
			break;
		case WSAEMSGSIZE:
			errno = EMSGSIZE;
			break;
		case WSAEPROTOTYPE:
			errno = EPROTOTYPE;
			break;
		case WSAENOPROTOOPT:
			errno = ENOPROTOOPT;
			break;
		case WSAEPROTONOSUPPORT:
			errno = EPROTONOSUPPORT;
			break;
		case WSAESOCKTNOSUPPORT:
			errno = ESOCKTNOSUPPORT;
			break;
		case WSAEOPNOTSUPP:
			errno = EOPNOTSUPP;
			break;
		case WSAEPFNOSUPPORT:
			errno = EPFNOSUPPORT;
			break;
		case WSAEAFNOSUPPORT:
			errno = EAFNOSUPPORT;
			break;
		case WSAEADDRINUSE:
			errno = EADDRINUSE;
			break;
		case WSAEADDRNOTAVAIL:
			errno = EADDRNOTAVAIL;
			break;
		case WSAENETDOWN:
			errno = ENETDOWN;
			break;
		case WSAENETUNREACH:
			errno = ENETUNREACH;
			break;
		case WSAENETRESET:
			errno = ENETRESET;
			break;
		case WSAECONNABORTED:
			errno = ECONNABORTED;
			break;
		case WSAECONNRESET:
			errno = ECONNRESET;
			break;
		case WSAENOBUFS:
			errno = ENOBUFS;
			break;
		case WSAEISCONN:
			errno = EISCONN;
			break;
		case WSAENOTCONN:
			errno = ENOTCONN;
			break;
		case WSAESHUTDOWN:
			errno = ESHUTDOWN;
			break;
		case WSAETOOMANYREFS:
			errno = ETOOMANYREFS;
			break;
		case WSAETIMEDOUT:
			errno = ETIMEDOUT;
			break;
		case WSAECONNREFUSED:
			errno = ECONNREFUSED;
			break;
		case WSAELOOP:
			errno = ELOOP;
			break;
		case WSAENAMETOOLONG:
			errno = ENAMETOOLONG;
			break;
		case WSAEHOSTDOWN:
			errno = EHOSTDOWN;
			break;
		case WSAEHOSTUNREACH:
			errno = EHOSTUNREACH;
			break;
		case WSAENOTEMPTY:
			errno = ENOTEMPTY;
			break;
#ifdef EPROCLIM
		case WSAEPROCLIM:
			errno = EPROCLIM;
			break;
#endif
		case WSAEUSERS:
			errno = EUSERS;
			break;
		case WSAEDQUOT:
			errno = EDQUOT;
			break;
		case WSAESTALE:
			errno = ESTALE;
			break;
		case WSAEREMOTE:
			errno = EREMOTE;
			break;
	}
}

int WSAGetLastError(void)
{
	int iError = 0;

	switch (errno)
	{
		/* Base error codes */

		case EINTR:
			iError = WSAEINTR;
			break;
		case EBADF:
			iError = WSAEBADF;
			break;
		case EACCES:
			iError = WSAEACCES;
			break;
		case EFAULT:
			iError = WSAEFAULT;
			break;
		case EINVAL:
			iError = WSAEINVAL;
			break;
		case EMFILE:
			iError = WSAEMFILE;
			break;

		/* BSD sockets error codes */

		case EWOULDBLOCK:
			iError = WSAEWOULDBLOCK;
			break;
		case EINPROGRESS:
			iError = WSAEINPROGRESS;
			break;
		case EALREADY:
			iError = WSAEALREADY;
			break;
		case ENOTSOCK:
			iError = WSAENOTSOCK;
			break;
		case EDESTADDRREQ:
			iError = WSAEDESTADDRREQ;
			break;
		case EMSGSIZE:
			iError = WSAEMSGSIZE;
			break;
		case EPROTOTYPE:
			iError = WSAEPROTOTYPE;
			break;
		case ENOPROTOOPT:
			iError = WSAENOPROTOOPT;
			break;
		case EPROTONOSUPPORT:
			iError = WSAEPROTONOSUPPORT;
			break;
		case ESOCKTNOSUPPORT:
			iError = WSAESOCKTNOSUPPORT;
			break;
		case EOPNOTSUPP:
			iError = WSAEOPNOTSUPP;
			break;
		case EPFNOSUPPORT:
			iError = WSAEPFNOSUPPORT;
			break;
		case EAFNOSUPPORT:
			iError = WSAEAFNOSUPPORT;
			break;
		case EADDRINUSE:
			iError = WSAEADDRINUSE;
			break;
		case EADDRNOTAVAIL:
			iError = WSAEADDRNOTAVAIL;
			break;
		case ENETDOWN:
			iError = WSAENETDOWN;
			break;
		case ENETUNREACH:
			iError = WSAENETUNREACH;
			break;
		case ENETRESET:
			iError = WSAENETRESET;
			break;
		case ECONNABORTED:
			iError = WSAECONNABORTED;
			break;
		case ECONNRESET:
			iError = WSAECONNRESET;
			break;
		case ENOBUFS:
			iError = WSAENOBUFS;
			break;
		case EISCONN:
			iError = WSAEISCONN;
			break;
		case ENOTCONN:
			iError = WSAENOTCONN;
			break;
		case ESHUTDOWN:
			iError = WSAESHUTDOWN;
			break;
		case ETOOMANYREFS:
			iError = WSAETOOMANYREFS;
			break;
		case ETIMEDOUT:
			iError = WSAETIMEDOUT;
			break;
		case ECONNREFUSED:
			iError = WSAECONNREFUSED;
			break;
		case ELOOP:
			iError = WSAELOOP;
			break;
		case ENAMETOOLONG:
			iError = WSAENAMETOOLONG;
			break;
		case EHOSTDOWN:
			iError = WSAEHOSTDOWN;
			break;
		case EHOSTUNREACH:
			iError = WSAEHOSTUNREACH;
			break;
		case ENOTEMPTY:
			iError = WSAENOTEMPTY;
			break;
#ifdef EPROCLIM
		case EPROCLIM:
			iError = WSAEPROCLIM;
			break;
#endif
		case EUSERS:
			iError = WSAEUSERS;
			break;
		case EDQUOT:
			iError = WSAEDQUOT;
			break;
		case ESTALE:
			iError = WSAESTALE;
			break;
		case EREMOTE:
			iError = WSAEREMOTE;
			break;

		/* Special cases */

#if (EAGAIN != EWOULDBLOCK)
		case EAGAIN:
			iError = WSAEWOULDBLOCK;
			break;
#endif

#if defined(EPROTO)
		case EPROTO:
			iError = WSAECONNRESET;
			break;
#endif
	}

	/**
	 * Windows Sockets Extended Error Codes:
	 *
	 * WSASYSNOTREADY
	 * WSAVERNOTSUPPORTED
	 * WSANOTINITIALISED
	 * WSAEDISCON
	 * WSAENOMORE
	 * WSAECANCELLED
	 * WSAEINVALIDPROCTABLE
	 * WSAEINVALIDPROVIDER
	 * WSAEPROVIDERFAILEDINIT
	 * WSASYSCALLFAILURE
	 * WSASERVICE_NOT_FOUND
	 * WSATYPE_NOT_FOUND
	 * WSA_E_NO_MORE
	 * WSA_E_CANCELLED
	 * WSAEREFUSED
	 */

	return iError;
}

HANDLE WSACreateEvent(void)
{
	return CreateEvent(NULL, TRUE, FALSE, NULL);
}

BOOL WSASetEvent(HANDLE hEvent)
{
	return SetEvent(hEvent);
}

BOOL WSAResetEvent(HANDLE hEvent)
{
	/* POSIX systems auto reset the socket,
	 * if no more data is available. */
	return TRUE;
}

BOOL WSACloseEvent(HANDLE hEvent)
{
	BOOL status;

	status = CloseHandle(hEvent);

	if (!status)
		SetLastError(6);

	return status;
}

int WSAEventSelect(SOCKET s, WSAEVENT hEventObject, LONG lNetworkEvents)
{
	u_long arg = 1;
	ULONG mode = 0;

	if (_ioctlsocket(s, FIONBIO, &arg) != 0)
		return SOCKET_ERROR;

	if (arg == 0)
		return 0;

	if (lNetworkEvents & FD_READ)
		mode |= UZI_FD_READ;
	if (lNetworkEvents & FD_WRITE)
		mode |= UZI_FD_WRITE;

	if (SetEventFileDescriptor(hEventObject, s, mode) < 0)
		return SOCKET_ERROR;

	return 0;
}

SOCKET _accept(SOCKET s, struct sockaddr* addr, int* addrlen)
{
	int status;
	int fd = (int) s;
	socklen_t s_addrlen = (socklen_t) *addrlen;

	status = accept(fd, addr, &s_addrlen);
	*addrlen = (socklen_t) s_addrlen;

	return status;
}

int _bind(SOCKET s, const struct sockaddr* addr, int namelen)
{
	int status;
	int fd = (int) s;

	status = bind(fd, addr, (socklen_t) namelen);

	if (status < 0)
		return SOCKET_ERROR;

	return status;
}

int _closesocket(SOCKET s)
{
	int status;
	int fd = (int) s;

	status = close(fd);

	return status;
}

int _connect(SOCKET s, const struct sockaddr* name, int namelen)
{
	int status;
	int fd = (int) s;

	status = connect(fd, name, (socklen_t) namelen);

	if (status < 0)
		return SOCKET_ERROR;

	return status;
}

int _ioctlsocket(SOCKET s, long cmd, u_long* argp)
{
	int fd = (int) s;

	if (cmd == FIONBIO)
	{
		int flags;

		if (!argp)
			return SOCKET_ERROR;

		flags = fcntl(fd, F_GETFL);

		if (flags == -1)
			return SOCKET_ERROR;

		if (*argp)
			fcntl(fd, F_SETFL, flags | O_NONBLOCK);
		else
			fcntl(fd, F_SETFL, flags & ~(O_NONBLOCK));
	}

	return 0;
}

int _getpeername(SOCKET s, struct sockaddr* name, int* namelen)
{
	int status;
	int fd = (int) s;
	socklen_t s_namelen = (socklen_t) *namelen;

	status = getpeername(fd, name, &s_namelen);
	*namelen = (int) s_namelen;

	return status;
}

int _getsockname(SOCKET s, struct sockaddr* name, int* namelen)
{
	int status;
	int fd = (int) s;
	socklen_t s_namelen = (socklen_t) *namelen;

	status = getsockname(fd, name, &s_namelen);
	*namelen = (int) s_namelen;

	return status;
}

int _getsockopt(SOCKET s, int level, int optname, char* optval, int* optlen)
{
	int status;
	int fd = (int) s;
	socklen_t s_optlen = (socklen_t) *optlen;

	status = getsockopt(fd, level, optname, (void*) optval, &s_optlen);
	*optlen = (socklen_t) s_optlen;

	return status;
}

u_long _htonl(u_long hostlong)
{
	return htonl(hostlong);
}

u_short _htons(u_short hostshort)
{
	return htons(hostshort);
}

unsigned long _inet_addr(const char* cp)
{
	return (long) inet_addr(cp);
}

char* _inet_ntoa(struct in_addr in)
{
	return inet_ntoa(in);
}

int _listen(SOCKET s, int backlog)
{
	int status;
	int fd = (int) s;

	status = listen(fd, backlog);

	return status;
}

u_long _ntohl(u_long netlong)
{
	return ntohl(netlong);
}

u_short _ntohs(u_short netshort)
{
	return ntohs(netshort);
}

int _recv(SOCKET s, char* buf, int len, int flags)
{
	int status;
	int fd = (int) s;

	status = (int) recv(fd, (void*) buf, (size_t) len, flags);

	return status;
}

int _recvfrom(SOCKET s, char* buf, int len, int flags, struct sockaddr* from, int* fromlen)
{
	int status;
	int fd = (int) s;
	socklen_t s_fromlen = (socklen_t) *fromlen;

	status = (int) recvfrom(fd, (void*) buf, (size_t) len, flags, from, &s_fromlen);
	*fromlen = (int) s_fromlen;

	return status;
}

int _select(int nfds, fd_set* readfds, fd_set* writefds, fd_set* exceptfds, const struct timeval* timeout)
{
	int status;

	do
	{
		status = select(nfds, readfds, writefds, exceptfds, (struct timeval*) timeout);
	}
	while ((status < 0) && (errno == EINTR));

	return status;
}

int _send(SOCKET s, const char* buf, int len, int flags)
{
	int status;
	int fd = (int) s;

	flags |= MSG_NOSIGNAL;

	status = (int) send(fd, (void*) buf, (size_t) len, flags);

	return status;
}

int _sendto(SOCKET s, const char* buf, int len, int flags, const struct sockaddr* to, int tolen)
{
	int status;
	int fd = (int) s;

	status = (int) sendto(fd, (void*) buf, (size_t) len, flags, to, (socklen_t) tolen);

	return status;
}

int _setsockopt(SOCKET s, int level, int optname, const char* optval, int optlen)
{
	int status;
	int fd = (int) s;

	status = setsockopt(fd, level, optname, (void*) optval, (socklen_t) optlen);

	return status;
}

int _shutdown(SOCKET s, int how)
{
	int status;
	int fd = (int) s;
	int s_how = -1;

	switch (how)
	{
		case SD_RECEIVE:
			s_how = SHUT_RD;
			break;

		case SD_SEND:
			s_how = SHUT_WR;
			break;

		case SD_BOTH:
			s_how = SHUT_RDWR;
			break;
	}

	if (s_how < 0)
		return SOCKET_ERROR;

	status = shutdown(fd, s_how);

	return status;
}

SOCKET _socket(int af, int type, int protocol)
{
	int fd;
	SOCKET s;

	fd = socket(af, type, protocol);

	if (fd < 1)
		return INVALID_SOCKET;

	s = (SOCKET) fd;

	return s;
}

struct hostent* _gethostbyaddr(const char* addr, int len, int type)
{
	struct hostent* host;

	host = gethostbyaddr((void*) addr, (socklen_t) len, type);

	return host;
}

struct hostent* _gethostbyname(const char* name)
{
	struct hostent* host;

	host = gethostbyname(name);

	return host;
}

int _gethostname(char* name, int namelen)
{
	int status;

	status = gethostname(name, (size_t) namelen);

	return status;
}

struct servent* _getservbyport(int port, const char* proto)
{
	struct servent* serv;

	serv = getservbyport(port, proto);

	return serv;
}

struct servent* _getservbyname(const char* name, const char* proto)
{
	struct servent* serv;

	serv = getservbyname(name, proto);

	return serv;
}

struct protoent* _getprotobynumber(int number)
{
	struct protoent* proto;

	proto = getprotobynumber(number);

	return proto;
}

struct protoent* _getprotobyname(const char* name)
{
	struct protoent* proto;

	proto = getprotobyname(name);

	return proto;
}

#endif  /* _WIN32 */
