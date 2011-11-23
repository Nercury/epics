add_executable(epics-proxy
	proxy_main.cpp)

target_link_libraries(epics-proxy
key-common
key-net
)