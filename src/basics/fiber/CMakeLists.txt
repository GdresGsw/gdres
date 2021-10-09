target_sources(wbasics 
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/fiber.cpp
        ${CMAKE_CURRENT_LIST_DIR}/scheduler.cpp
)

target_include_directories(wbasics
    PUBLIC 
        ${CMAKE_CURRENT_LIST_DIR}
)
