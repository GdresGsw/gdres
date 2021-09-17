add_library(study_prictice "")

add_subdirectory(thread)
add_subdirectory(study)

target_include_directories(study_prictice
    PUBLIC 
        ${CMAKE_CURRENT_LIST_DIR}
)