target_sources(wbasics 
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/modul_mgr.cpp
        ${CMAKE_CURRENT_LIST_DIR}/modul_base.cpp
        ${CMAKE_CURRENT_LIST_DIR}/msgqueue.cpp
        ${CMAKE_CURRENT_LIST_DIR}/modul_timer.cpp
        ${CMAKE_CURRENT_LIST_DIR}/modul_test1.cpp
        ${CMAKE_CURRENT_LIST_DIR}/modul_test2.cpp
        ${CMAKE_CURRENT_LIST_DIR}/modul_test3.cpp
)

target_include_directories(wbasics
    PUBLIC 
        ${CMAKE_CURRENT_LIST_DIR}
)
