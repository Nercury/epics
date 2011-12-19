#include "Socket.h"

using namespace key;
using namespace key::internal;

void Socket::Init(KEY_SOCKET socket_handle) 
{
#ifdef WIN32
	WSADATA wsaData;
	if (WSAStartup( MAKEWORD(2,2), &wsaData ) != NO_ERROR)
	{
		is_failed = true;
		return;
	}
#endif

	this->af = AF_INET;
	this->type = SOCK_STREAM;
	this->protocol = IPPROTO_TCP;

	if (socket_handle == NULL) {
		this->socket_handle = socket(this->af, this->type, this->protocol);
		
		if ( this->socket_handle == INVALID_SOCKET ) {
			is_failed = true;
		} else {
			u_long iMode = 1;
			ioctlsocket(this->socket_handle, FIONBIO, &iMode);
		}
	} else {
		this->socket_handle = socket_handle;
		if (this->socket_handle != INVALID_SOCKET && this->socket_handle != SOCKET_ERROR)
			was_connected = true;
	}

#ifdef WIN32
	if (this->socket_handle != NULL)
		this->ex_functions.set_socket(this->socket_handle);
#endif
}

Socket::Socket() :
	is_bound(false), 
	is_failed(false), 
	is_listening(false), 
	was_connected(false),
	is_accepting(false),
	is_accepting_and_receiving(false)
{
	this->Init(NULL);
}

Socket::Socket(KEY_SOCKET socket_handle) :
	is_bound(false), 
	is_failed(false), 
	is_listening(false), 
	was_connected(false),
	is_accepting(false),
	is_accepting_and_receiving(false)
{
	this->Init(socket_handle);
}

Socket::~Socket()
{
	this->Close();
#ifdef WIN32
	// only the last call to this cleans up wsa, so we are good.
	WSACleanup();
#endif
}

fun_res Socket::Bind(uint16_t port)
{
	this->service.sin_family = AF_INET;
	this->service.sin_addr.s_addr = ADDR_ANY;//inet_addr("0.0.0.0");
	this->service.sin_port = htons(port);

	int yes = 1;
	if (setsockopt(this->socket_handle, SOL_SOCKET, SO_REUSEADDR, (char*)&yes, sizeof(yes)) < 0)
	{
		std::string error = (boost::format("Error setting socket parameters: %1%") % socket_error_message(socket_last_error(), SocketErrorScope::Bind)).str(); 
		closesocket(this->socket_handle);
		this->socket_handle = SOCKET_ERROR;
		this->is_bound = false;
		return fun_error(error);
	}

	// Bind the socket.
	auto res = bind( this->socket_handle, (SOCKADDR*)&(this->service), sizeof(this->service));
	if (res == SOCKET_ERROR) {
		std::string error = (boost::format("Error binding socket: %1%") % socket_error_message(socket_last_error(), SocketErrorScope::Bind)).str(); 
		closesocket(this->socket_handle);
		this->socket_handle = SOCKET_ERROR;
		this->is_bound = false;
		return fun_error(error);
	}

	this->is_bound = true;
	return fun_ok();
}

#ifdef WIN32
static void begin_accept_win_completion(DWORD bytes_transfered, key::internal::OVERLAPPEDACCEPT * ac)
{
	ac->callback(std::make_shared<Socket>(ac->accept_socket));
}
#endif

fun_res Socket::BeginAccept(std::function<void (std::shared_ptr<Socket>)> callback)
{
	if (is_failed)
		return fun_error("Cannot accept on failed socket");

	if (!is_bound)
		return fun_error("Bind() must be called before BeginAccept()");

	if (!this->is_listening)
	{
		this->is_listening = true;
		if (listen( this->socket_handle, 1 ) == SOCKET_ERROR)
		{
			this->is_listening = false;
			return fun_error((boost::format("Error listening on socket: %1%") % socket_error_message(socket_last_error(), SocketErrorScope::Listen)).str());
		}
	}

#ifdef WIN32
	if (!is_accepting) {
		accept_completion.Open(boost::thread::hardware_concurrency(), begin_accept_win_completion);
		is_accepting = true;
	}

	KEY_SOCKET acc_socket = socket(this->af, this->type, this->protocol);
	if ( acc_socket == INVALID_SOCKET ) {
		return fun_error(boost::format("Error creating accept socket: %1%") % socket_last_error_message());
	} else {
		u_long iMode = 1;
		ioctlsocket(acc_socket, FIONBIO, &iMode);

		accept_completion.AssignSocket(this->socket_handle);

		auto data = accept_completion.NewData();
		memset(&(data->ol), 0, sizeof(data->ol));
		data->accept_socket = acc_socket;
		data->callback = callback;

		ex_functions.get_accept_ex()(this->socket_handle, 
			acc_socket,
			&(*data).buf,
			0,
			sizeof(sockaddr_in) + 16,
			sizeof(sockaddr_in) + 16,
			&(data->bytesReceived),
			&(data->ol)); // uh...
	}

#endif

	return fun_ok();
}

#ifdef WIN32
static void begin_accept_and_receive_win_completion(DWORD bytes_transferred, key::internal::OVERLAPPEDACCEPTANDRECEIVE * ac)
{
	ac->callback((uint32_t)bytes_transferred, ac->buf.get(), std::make_shared<Socket>(ac->accept_socket));
}
#endif

fun_res Socket::BeginAcceptAndReceive(int32_t no_data_timeout_ms, int32_t buf_size, std::function<void (uint32_t bytes_transferred, char * bytes, std::shared_ptr<Socket>)> callback)
{
	if (is_failed)
		return fun_error("Cannot accept on failed socket");

	if (!is_bound)
		return fun_error("Bind() must be called before BeginAccept()");

	if (!this->is_listening)
	{
		this->is_listening = true;
		if (listen( this->socket_handle, 1 ) == SOCKET_ERROR)
		{
			this->is_listening = false;
			return fun_error((boost::format("Error listening on socket: %1%") % socket_error_message(socket_last_error(), SocketErrorScope::Listen)).str());
		}
	}

#ifdef WIN32
	if (!is_accepting_and_receiving) {
		accept_and_receive_completion.Open(boost::thread::hardware_concurrency(), begin_accept_and_receive_win_completion);
		is_accepting_and_receiving = true;
	}

	KEY_SOCKET acc_socket = socket(this->af, this->type, this->protocol);
	if ( acc_socket == INVALID_SOCKET ) {
		return fun_error(boost::format("Error creating accept socket: %1%") % socket_last_error_message());
	} else {
		u_long iMode = 1;
		ioctlsocket(acc_socket, FIONBIO, &iMode);

		accept_and_receive_completion.AssignSocket(this->socket_handle);

		auto data = accept_and_receive_completion.NewData();
		memset(&(data->ol), 0, sizeof(data->ol));
		data->accept_socket = acc_socket;
		data->buf.reset(new char[buf_size + (sizeof(sockaddr_in) + 16) * 2]);
		data->buf_size = buf_size;
		data->callback = callback;

		ex_functions.get_accept_ex()(this->socket_handle, 
			acc_socket,
			&(*data).buf[0],
			buf_size,
			sizeof(sockaddr_in) + 16,
			sizeof(sockaddr_in) + 16,
			&(data->bytesReceived),
			&(data->ol)); // uh...
	}

#endif

	return fun_ok();
}

fun_res Socket::BeginConnect(std::string address, uint16_t port, std::function<void (std::shared_ptr<Socket>)> callback)
{
	if (is_failed)
		return fun_error("Cannot connect on failed socket");

	if (was_connected)
		return fun_error("Can not connect on already connected socket");

#ifdef WIN32
	auto connect_ex = this->ex_functions.get_connect_ex();
	return cndc->BeginConnect(connect_ex, this->shared_from_this(), address, port, callback);
#else
	return fun_error("connect is not implemented");
#endif
}

fun_res Socket::BeginSend(char * buffer, int buffer_size, std::function<void (std::shared_ptr<Socket>)> callback)
{
	if (is_failed)
		return fun_error("Cannot send on failed socket");

	if (!was_connected)
		return fun_error("Cannot send on non-connected socket");

#ifdef WIN32
	return this->io->BeginSend(this->shared_from_this(), buffer, buffer_size, callback);
#else
	return fun_error("send is not implemented");
#endif
}

fun_res Socket::BeginReceive(char * buffer, int buffer_size, std::function<void (char * buffer, int buffer_size, uint64_t bytes_transferred, std::shared_ptr<Socket>)> callback)
{
	if (is_failed)
		return fun_error("Cannot receive on failed socket");

	if (!was_connected)
		return fun_error("Cannot receive on non-connected socket");

#ifdef WIN32
	return this->io->BeginReceive(this->shared_from_this(), buffer, buffer_size, callback);
#else
	return fun_error("receive is not implemented");
#endif
}

fun_res Socket::Close()
{
	this->was_connected = false;
	if (this->socket_handle != INVALID_SOCKET) {
		if (closesocket(this->socket_handle) == SOCKET_ERROR)
		{
			this->socket_handle = INVALID_SOCKET;
			return fun_error(boost::format("Socket close failure: %1%") % socket_last_error_message());
		}
		this->socket_handle = INVALID_SOCKET;

		#ifdef WIN32
			this->accept_completion.Close();
			this->accept_and_receive_completion.Close();
		#endif
		this->is_accepting = false;
		this->is_accepting_and_receiving = false;
	}

	return fun_ok();
}

