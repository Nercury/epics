#include "win_socket_errors.h"

#include <boost/format.hpp>

using namespace key::internal;

int key::internal::socket_last_error()
{
	return WSAGetLastError();
}

std::string key::internal::socket_error_message(int error_num, key::internal::SocketErrorScope scope) {
	switch (error_num)
	{
	case WSAENOTSOCK:
		return "An operation was attempted on something that is not a socket.";

	case WSAEINVAL:
		switch (scope)
		{
		case SocketErrorScope::Bind:
			return "Socket is already bound to an address";
		case SocketErrorScope::Accept:
			return "listen() was not invoked prior to accept()";
		case SocketErrorScope::Listen:
			return "Socket not bound with bind() or already connected";
		default:
			return "Some invalid argument was supplied";
		}

	case WSAEADDRINUSE:
		return "Address already in use";

	case WSAEACCES:
		switch (scope)
		{
		case SocketErrorScope::Bind:
			return "The port is most likely in use by another service";
		default:
			return "Permission denied";
		}

	case WSAECONNABORTED:
		return "Software caused connection abort";

	case WSAECONNREFUSED:
		return "Connection refused";

	case WSAECONNRESET:
		return "Connection reset by peer";

	case WSAEDESTADDRREQ:
		return "Destination address required";

	case WSAEHOSTUNREACH:
		return "No route to host";

	case WSAEMFILE:
		return "Too many open files";

	case WSAENETDOWN:
		return "Network is down";

	case WSAENETRESET:
		return "Network dropped connection";

	case WSAENOBUFS:
		return "No buffer space available";

	case WSAENETUNREACH:
		return "Network is unreachable";

	case WSAETIMEDOUT:
		return "Connection timed out";

	case WSAHOST_NOT_FOUND:
		return "Host not found";

	case WSASYSNOTREADY:
		return "Network sub-system is unavailable";

	case WSANOTINITIALISED:
		return "WSAStartup() not performed";

	case WSANO_DATA:
		return "Valid name, no data of that type";

	case WSANO_RECOVERY:
		return "Non-recoverable query error";

	case WSATRY_AGAIN:
		return "Non-authoritative host found";

	case WSAVERNOTSUPPORTED:
		return "Wrong WinSock DLL version";

	default:
		return (boost::format("Unknown Winsock error %1%") % error_num).str();
	}
}