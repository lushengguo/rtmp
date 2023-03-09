add_executable(rtmp-server server.cpp)
target_link_libraries(rtmp-server net::library base::library)