#pragma once

#include <key-common/platform.hpp>

#if defined(WIN32) || defined(_WINDOWS)
#include "mswsock.h" // ms socket extensions
#endif

typedef SOCKET KEY_SOCKET;