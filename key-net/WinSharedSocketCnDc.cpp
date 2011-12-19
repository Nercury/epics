#include "WinSharedSocketCnDc.h"

#include <key-common/sockets.hpp>
#include <key-net/Socket.h>
#include <key-net/win_socket_errors.h>

#include <iostream>

using namespace key;

WinSharedSocketCnDc::WinSharedSocketCnDc() :
	is_cndc_created(false)
{

}

WinSharedSocketCnDc::~WinSharedSocketCnDc()
{
	if (is_cndc_created)
		cndc_completion.Close();
}

#ifdef WIN32
static void begin_simple_io_win_completion(DWORD bytes_transferred, key::internal::OVERLAPPEDCNDC * ac)
{
	if (ac->read)
	{
		DWORD recbytes;
		DWORD dwFlags;

		if (WSAGetOverlappedResult(
			ac->socket->GetHandle(),
			&(ac->ol),
			&recbytes,
			TRUE,
			&dwFlags) == TRUE)
		{
			ac->recv_callback(ac->buffer.buf, ac->buffer.len, (uint64_t)recbytes, ac->socket);
		}
		else
		{
			std::cout << "WSAGetOverlappedResult for oi overlapped socket failed. Someone should fix this." << std::endl; 
		}
	}
	else
		ac->send_callback(ac->socket);
}
#endif

fun_res WinSharedSocketCnDc::BeginConnect(LPFN_CONNECTEX function, std::shared_ptr<Socket> socket, std::string & address, uint16_t port, std::function<void (std::shared_ptr<Socket>)> & callback)
{
	if (!is_cndc_created) {
		cndc_completion.Open(boost::thread::hardware_concurrency(), begin_simple_io_win_completion);
		is_cndc_created = true;
	}

	cndc_completion.AssignSocket(socket->GetHandle());

	auto data = cndc_completion.NewData();
	memset(&(data->ol), 0, sizeof(data->ol));
	data->socket = socket;
	data->buffer.buf = NULL;
	data->buffer.len = 0;
	data->callback = callback;
	data->connect_and_send_callback = false;
	


	auto ret = function(socket->GetHandle(), &(data->buffer), 1, NULL, 0, &(data->ol), NULL);
	if (ret == 0) { // completed synchronously
		// overlapped stuff is still called as usual, so do not delete data here
		return fun_ok();
	} else {
		int error = WSAGetLastError();
		if (error == WSA_IO_PENDING)
			// if error is not error, return ok
			return fun_ok();
		else
		{
			cndc_completion.DeleteData(data);
			return fun_error(boost::format("WSASend failed to start: %1%") % internal::socket_error_message(error, internal::SocketErrorScope::Any));
		}
	}
}

/*fun_res WinSharedSocketCnDc::BeginReceive(std::shared_ptr<Socket> socket, char * buffer, int buffer_size, std::function<void (char * buffer, int buffer_size, uint64_t bytes_transferred, std::shared_ptr<Socket>)> & callback)
{
	if (!is_cndc_created) {
		cndc_completion.Open(boost::thread::hardware_concurrency(), begin_simple_io_win_completion);
		is_cndc_created = true;
	}

	cndc_completion.AssignSocket(socket->GetHandle());

	auto data = cndc_completion.NewData();
	memset(&(data->ol), 0, sizeof(data->ol));
	data->socket = socket;
	data->buffer.buf = buffer;
	data->buffer.len = buffer_size;
	data->recv_callback = callback;
	data->read = true;
	
	DWORD flags = 0;

	auto ret = WSARecv(socket->GetHandle(), &(data->buffer), 1, NULL, &flags, &(data->ol), NULL);
	if (ret == 0) { // completed synchronously
		// overlapped stuff is still called as usual, so do not delete data here
		return fun_ok();
	} else {
		int error = WSAGetLastError();
		if (error == WSA_IO_PENDING)
			// if error is not error, return ok
			return fun_ok();
		else
		{
			cndc_completion.DeleteData(data);
			return fun_error(boost::format("WSARecv failed to start: %1%") % internal::socket_error_message(error, internal::SocketErrorScope::Any));
		}
	}
}*/