add_executable(epics-auth
	auth_main.cpp)

target_link_libraries(epics-auth
key-common
key-net
)