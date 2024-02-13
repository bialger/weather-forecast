add_library(utils STATIC
        utils.cpp
        utils.hpp
)

target_include_directories(utils PUBLIC ${PROJECT_SOURCE_DIR})
