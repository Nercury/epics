#pragma once

#include <key-common/sockets.hpp>

namespace key 
{
	class WinWsaSocketFunctions
	{
	private:
		KEY_SOCKET socket;

		LPFN_ACCEPTEX accept_ex;
		LPFN_CONNECTEX connect_ex;
		LPFN_DISCONNECTEX disconnect_ex;
	public:
		WinWsaSocketFunctions();
		~WinWsaSocketFunctions();

		void set_socket(KEY_SOCKET socket);
		void invalidate();

		LPFN_ACCEPTEX get_accept_ex();
		LPFN_CONNECTEX get_connect_ex();
		LPFN_DISCONNECTEX get_disconnect_ex();
	};
}