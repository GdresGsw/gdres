target_sources(wbasics 
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/timer.cpp
)

target_include_directories(wbasics
    PUBLIC 
        ${CMAKE_CURRENT_LIST_DIR}
)
