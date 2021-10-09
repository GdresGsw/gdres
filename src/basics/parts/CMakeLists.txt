target_sources(wbasics 
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/file.cpp
        ${CMAKE_CURRENT_LIST_DIR}/lock.cpp
        ${CMAKE_CURRENT_LIST_DIR}/func_pub.cpp
)

target_include_directories(wbasics
    PUBLIC 
        ${CMAKE_CURRENT_LIST_DIR}
)
