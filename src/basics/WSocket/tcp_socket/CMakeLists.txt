target_sources(Wbasics 
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/sock_p.cpp
        ${CMAKE_CURRENT_LIST_DIR}/socket_client.cpp
        ${CMAKE_CURRENT_LIST_DIR}/cell_server.cpp
        ${CMAKE_CURRENT_LIST_DIR}/client_cell.cpp
        ${CMAKE_CURRENT_LIST_DIR}/tcp_server.cpp
)

target_include_directories(Wbasics
    PUBLIC 
        ${CMAKE_CURRENT_LIST_DIR}
)