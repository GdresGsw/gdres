target_sources(mytest 
    PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/log_test.cpp
    ${CMAKE_CURRENT_LIST_DIR}/threadpool_test.cpp
    ${CMAKE_CURRENT_LIST_DIR}/fiber_test.cpp
    ${CMAKE_CURRENT_LIST_DIR}/modulmgr_test.cpp
)

target_include_directories(mytest
    PUBLIC 
        ${CMAKE_CURRENT_LIST_DIR}
)

