cmake_minimum_required(VERSION 3.12)

project(
        weather-forecast
        VERSION 1.1
        DESCRIPTION "Console weather forecast app that uses OpenMeteo and Yandex Geocoder API"
        LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)

if (WIN32) # Install dlls in the same directory as the executable on Windows
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
    set(CMAKE_BUILD_TYPE Debug) # Release configuration fails to build
endif ()

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(CMAKE_CXX_FLAGS_DEBUG "/MDd")
    set(CMAKE_CXX_FLAGS_RELEASE "/O2")
else ()
    set(CMAKE_CXX_FLAGS_DEBUG "-g")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3")
endif ()

add_subdirectory(lib)
add_subdirectory(bin)


enable_testing()
add_subdirectory(tests)
