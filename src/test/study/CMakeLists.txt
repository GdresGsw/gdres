target_sources(study_prictice
    PRIVATE 
        ${CMAKE_CURRENT_LIST_DIR}/sp_hash.cpp
)

target_include_directories(study_prictice
    PUBLIC 
        ${CMAKE_CURRENT_LIST_DIR}
)