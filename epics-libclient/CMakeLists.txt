project(epics-libclient)

# if library is SHARED, uncoment following lines:
#if(WIN32)
# 	add_definitions(-DDYNAMIC_LIB)
#endif(WIN32)

add_library(epics-libclient STATIC 
	lib_epics_client.h 
	)

target_link_libraries(epics-libclient 
	key-common
	key-net
	)