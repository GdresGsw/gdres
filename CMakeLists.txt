cmake_minimum_required(VERSION 3.5)
project(MY_TEST LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)  
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED	ON)

include(GNUInstallDirs)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/bin)

# if(CMAKE_SYSTEM_NAME MATCHES "Windows")
#     set(SYSTEM_TYPE true)
# elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
#     set(SYSTEM_TYPE false)
# else()
#     message("this is other")
# endif()


add_subdirectory(src)
