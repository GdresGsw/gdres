add_subdirectory(basics)

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    add_executable(gdres gdres.cpp)
    add_subdirectory(test)
    target_link_libraries(gdres
        PRIVATE 
            study_prictice        
            ws2_32.lib #windows环境下使用socket需要连接该库
    )

elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
endif()