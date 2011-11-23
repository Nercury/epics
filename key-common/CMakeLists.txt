project(key-common)

# if library is SHARED, uncoment following lines:
#if(WIN32)
# 	add_definitions(-DDYNAMIC_LIB)
#endif(WIN32)

add_library(key-common STATIC 
	lib_key_common.h 
	platform.hpp # platform-specific includes (if you need windows.h)
	sockets.hpp
	types.h # common type definitions
	app.h # app inialization path, path separator, relative path function
	error.hpp # error object + message mechanism
	app.cpp 
	shared_instance.hpp
	shared_instance.inline.hpp
	JsonConfig.h # json configuration file
	JsonConfig.cpp
	)

target_link_libraries(key-common
	${JSON_LIBRARY}
	)