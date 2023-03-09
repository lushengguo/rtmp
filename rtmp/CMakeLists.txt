add_executable(rtmp-server server.cpp)
target_link_libraries(rtmp-server net::library base::library)

add_executable(rtmp-client client.cpp)
target_link_libraries(rtmp-client net::library base::library)