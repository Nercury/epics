#include "WinWsaSocketFunctions.h"

using namespace key;

WinWsaSocketFunctions::WinWsaSocketFunctions() :
	socket(NULL), accept_ex(NULL), connect_ex(NULL), disconnect_ex(NULL)
{

}

WinWsaSocketFunctions::~WinWsaSocketFunctions()
{
	this->invalidate();
}

void WinWsaSocketFunctions::set_socket(KEY_SOCKET socket)
{
	this->socket = socket;
	this->invalidate();
}

void WinWsaSocketFunctions::invalidate()
{
	this->accept_ex = NULL;
	this->connect_ex = NULL;
	this->disconnect_ex = NULL;
}

LPFN_ACCEPTEX WinWsaSocketFunctions::get_accept_ex()
{
	if (this->accept_ex != NULL)
		return this->accept_ex;

	if (this->socket == NULL)
		return NULL;

	GUID GuidAcceptEx = WSAID_ACCEPTEX;

	DWORD bytesReceived;

	// get a function pointer
	auto res = WSAIoctl(this->socket, SIO_GET_EXTENSION_FUNCTION_POINTER, &GuidAcceptEx, sizeof(GuidAcceptEx),
		&this->accept_ex, sizeof(this->accept_ex), &bytesReceived, NULL, NULL);

	if (res != NO_ERROR)
		this->accept_ex = NULL;

	return this->accept_ex;
}

LPFN_CONNECTEX WinWsaSocketFunctions::get_connect_ex()
{
	if (this->connect_ex != NULL)
		return this->connect_ex;

	if (this->socket == NULL)
		return NULL;

	GUID GuidConnectEx = WSAID_CONNECTEX;

	DWORD bytesReceived;

	// get a function pointer
	auto res = WSAIoctl(this->socket, SIO_GET_EXTENSION_FUNCTION_POINTER, &GuidConnectEx, sizeof(GuidConnectEx),
		&this->connect_ex, sizeof(this->connect_ex), &bytesReceived, NULL, NULL);

	if (res != NO_ERROR)
		this->connect_ex = NULL;

	return this->connect_ex;
}

LPFN_DISCONNECTEX WinWsaSocketFunctions::get_disconnect_ex()
{
	if (this->disconnect_ex != NULL)
		return this->disconnect_ex;

	if (this->socket == NULL)
		return NULL;

	GUID GuidDisconnectEx = WSAID_DISCONNECTEX;

	DWORD bytesReceived;

	// get a function pointer
	auto res = WSAIoctl(this->socket, SIO_GET_EXTENSION_FUNCTION_POINTER, &GuidDisconnectEx, sizeof(GuidDisconnectEx),
		&this->disconnect_ex, sizeof(this->disconnect_ex), &bytesReceived, NULL, NULL);

	if (res != NO_ERROR)
		this->disconnect_ex = NULL;

	return this->disconnect_ex;
}