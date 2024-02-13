cmake_minimum_required(VERSION 3.12)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug) # Change this to Release when you're ready to release
endif()

if(WIN32) # Install dlls in the same directory as the executable on Windows
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
endif()

message(STATUS "Libraries build type: ${CMAKE_BUILD_TYPE}")

add_subdirectory(argparser)
add_subdirectory(ui)
add_subdirectory(weather)
add_subdirectory(forecast)
add_subdirectory(utils)

add_subdirectory(ftxui)
add_subdirectory(cpr)
add_subdirectory(json)
