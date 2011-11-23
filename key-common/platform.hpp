#pragma once

#if defined(WIN32) || defined(_WINDOWS)
#	define WIN32_LEAN_AND_MEAN
#	define NOMINMAX
#	include <winsock2.h> // we have to inslude winsuck BEFORE windows.h
#	include <ws2tcpip.h>
#	include <windows.h>
#endif