target_sources(wbasics
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/log.cpp
)

target_include_directories(wbasics
    PUBLIC 
        ${CMAKE_CURRENT_LIST_DIR}
)
