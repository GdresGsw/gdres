target_sources(wbasics 
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/wthread.cpp
        ${CMAKE_CURRENT_LIST_DIR}/thread_pool.cpp
)

target_include_directories(wbasics
    PUBLIC 
        ${CMAKE_CURRENT_LIST_DIR}
)
