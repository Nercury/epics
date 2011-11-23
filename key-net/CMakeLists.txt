project(key-net)

# if library is SHARED, uncoment following lines:
#if(WIN32)
# 	add_definitions(-DDYNAMIC_LIB)
#endif(WIN32)

add_library(key-net STATIC 
	lib_key_net.h 
	win_socket_errors.h
	win_socket_errors.cpp
	WinCompletionPort.hpp
	WinCompletionPort.inline.hpp
	WinWsaSocketFunctions.h
	WinWsaSocketFunctions.cpp
	WinSharedSocketIo.h
	WinSharedSocketIo.cpp
	Socket.h
	Socket.cpp
	)

if(WIN32)
	SET(WINSOCK_LIBRARY Ws2_32.lib)
endif()

target_link_libraries(key-net 
	key-common
	${WINSOCK_LIBRARY}
	${Boost_LIBRARIES}
	)