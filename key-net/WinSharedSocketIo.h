#pragma once

#include <key-common/sockets.hpp>
#include <key-common/error.hpp>
#include <key-net/WinCompletionPort.hpp>

namespace key
{
	class Socket;

	namespace internal
	{
#ifdef WIN32
		typedef struct _OVERLAPPEDSIMPLEIO {
			OVERLAPPED	ol;
			std::shared_ptr<Socket>	socket;
			std::function<void (std::shared_ptr<Socket>)> send_callback;
			std::function<void (char * buffer, int buffer_size, uint64_t bytes_transferred, std::shared_ptr<Socket>)> recv_callback;
			WSABUF		buffer;
			bool		read; /* if true, data is for reading, otherwise, for writing */
		} OVERLAPPEDSIMPLEIO;
#endif
	}

	class WinSharedSocketIo
	{
	private:
		WinCompletionPort<internal::_OVERLAPPEDSIMPLEIO> simple_io_completion;
		bool is_simple_io;
	public:
		WinSharedSocketIo();
		~WinSharedSocketIo();

		fun_res BeginSend(std::shared_ptr<Socket> socket, char * buffer, int buffer_size, std::function<void (std::shared_ptr<Socket>)> & callback);
		fun_res BeginReceive(std::shared_ptr<Socket> socket, char * buffer, int buffer_size, std::function<void (char * buffer, int buffer_size, uint64_t bytes_transferred, std::shared_ptr<Socket>)> & callback);
	};
}