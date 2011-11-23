add_executable(epics-masterserver
	master_main.cpp)

target_link_libraries(epics-masterserver
key-common
key-net
)