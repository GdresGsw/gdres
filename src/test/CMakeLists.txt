add_executable(mytest "")
# add_executable(server server.cpp)
# add_executable(client client.cpp)

target_sources(mytest 
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/mytest.cpp
)

add_subdirectory(mytest)

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
target_link_libraries(mytest PRIVATE wbasics)
# target_link_libraries(server PRIVATE Wbasics ws2_32.lib)
# target_link_libraries(client PRIVATE Wbasics ws2_32.lib)
elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
target_link_libraries(mytest PRIVATE wbasics pthread)
# target_link_libraries(server PRIVATE Wbasics pthread)
# target_link_libraries(client PRIVATE Wbasics pthread)
endif()
