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
		typedef struct _OVERLAPPEDCNDC {
			OVERLAPPED	ol;
			std::shared_ptr<Socket>	socket;
			std::function<void (std::shared_ptr<Socket>)> callback;
			std::function<void (char * buffer, int buffer_size, uint64_t bytes_transferred, std::shared_ptr<Socket>)> connect_and_send_callback;
			WSABUF		buffer;
			bool		connect_and_send; /* if true, connected with send */
		} OVERLAPPEDCNDC;
#endif
	}

	class WinSharedSocketCnDc
	{
	private:
		WinCompletionPort<internal::OVERLAPPEDCNDC> cndc_completion;
		bool is_cndc_created;
	public:
		WinSharedSocketCnDc();
		~WinSharedSocketCnDc();

		fun_res BeginConnect(LPFN_CONNECTEX function, std::shared_ptr<Socket> socket, std::string & address, uint16_t port, std::function<void (std::shared_ptr<Socket>)> & callback);
		//fun_res BeginReceive(std::shared_ptr<Socket> socket, char * buffer, int buffer_size, std::function<void (char * buffer, int buffer_size, uint64_t bytes_transferred, std::shared_ptr<Socket>)> & callback);
	};
}