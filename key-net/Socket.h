#pragma once

#include <key-common/sockets.hpp>
#include <key-common/error.hpp>

#ifdef WIN32
#	include <key-net/WinCompletionPort.hpp>
#	include <key-net/WinWsaSocketFunctions.h>
#	include <key-net/WinSharedSocketIo.h>
#   include <key-net/WinSharedSocketCnDc.h>
#	include <key-net/win_socket_errors.h>
#	include <key-common/shared_instance.hpp>
#endif

#include <vector>
#include <boost/scoped_array.hpp>

namespace key {

	class Socket;

	namespace internal
	{
#ifdef WIN32
		typedef struct _OVERLAPPEDACCEPT {
			OVERLAPPED	ol;
			KEY_SOCKET	accept_socket;
			std::function<void (std::shared_ptr<Socket>)> callback;
			char		buf[(sizeof(sockaddr_in) + 16) * 2];
			DWORD		bytesReceived;
		} OVERLAPPEDACCEPT;

		typedef struct _OVERLAPPEDACCEPTANDRECEIVE {
			OVERLAPPED	ol;
			KEY_SOCKET	accept_socket;
			std::function<void (uint32_t bytes_transferred, char * bytes, std::shared_ptr<Socket>)> callback;
			boost::scoped_array<char>		buf;
			int32_t		buf_size;
			DWORD		bytesReceived;
		} OVERLAPPEDACCEPTANDRECEIVE;
#endif
	}

	class Socket : public std::enable_shared_from_this<Socket>
	{
	private:
#ifdef WIN32
		WinCompletionPort<internal::OVERLAPPEDACCEPT> accept_completion;
		WinCompletionPort<internal::OVERLAPPEDACCEPTANDRECEIVE> accept_and_receive_completion;
		shared_instance<WinSharedSocketIo> io;
		shared_instance<WinSharedSocketCnDc> cndc;
		WinWsaSocketFunctions ex_functions;
#endif
		bool is_bound; // bind was used on this
		bool is_failed;
		bool is_listening;
		bool was_connected;
		bool is_accepting; // using accept_completion
		bool is_accepting_and_receiving; // using accept_and_receive_completion

		// socket initialization parameters
		int af;
		int type;
		int protocol;

		sockaddr_in service; // service used for connection
		KEY_SOCKET socket_handle; // socket descriptor

		void Init(KEY_SOCKET sock);
	public:
		Socket();
		Socket(KEY_SOCKET socket_handle);
		~Socket();
		fun_res Bind(uint16_t port);
		fun_res BeginAccept(std::function<void (std::shared_ptr<Socket>)> callback);
		fun_res BeginAcceptAndReceive(int32_t no_data_timeout_ms, int32_t buf_size, std::function<void (uint32_t bytes_transferred, char * bytes, std::shared_ptr<Socket>)> callback);
		fun_res BeginSend(char * buffer, int buffer_size, std::function<void (std::shared_ptr<Socket>)> callback);
		fun_res BeginReceive(char * buffer, int buffer_size, std::function<void (char * buffer, int buffer_size, uint64_t bytes_transferred, std::shared_ptr<Socket>)> callback);
		fun_res BeginConnect(std::string address, uint16_t port, std::function<void (std::shared_ptr<Socket>)> callback);
		
		inline KEY_SOCKET GetHandle() { return this->socket_handle; }
		fun_res Close();
	};

}