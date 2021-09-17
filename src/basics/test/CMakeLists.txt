add_executable(threadTest mytest.cpp)
add_executable(server server.cpp)
add_executable(client client.cpp)


if(CMAKE_SYSTEM_NAME MATCHES "Windows")
target_link_libraries(threadTest PRIVATE Wbasics)
target_link_libraries(server PRIVATE Wbasics ws2_32.lib)
target_link_libraries(client PRIVATE Wbasics ws2_32.lib)
elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
target_link_libraries(threadTest PRIVATE pthread Wbasics)
target_link_libraries(server PRIVATE Wbasics pthread)
target_link_libraries(client PRIVATE Wbasics pthread)
endif()

