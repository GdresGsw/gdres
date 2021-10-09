target_sources(Wbasics 
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/walloctor.cpp
)

target_include_directories(Wbasics
    PUBLIC 
        ${CMAKE_CURRENT_LIST_DIR}
)