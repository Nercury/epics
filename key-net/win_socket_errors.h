#pragma once

#include <key-net/lib_key_net.h>
#include <key-common/sockets.hpp>

#include <string>

namespace key {
	namespace internal {
		enum class SocketErrorScope {
			Any,
			Bind,
			Listen,
			Accept,
		};

		/** returns last error for socket opperation */
		LIB_KEY_NET int socket_last_error();
		LIB_KEY_NET std::string socket_error_message(int error_num, SocketErrorScope scope);
		inline std::string socket_last_error_message() { return socket_error_message(socket_last_error(), SocketErrorScope::Any); }
	}
}