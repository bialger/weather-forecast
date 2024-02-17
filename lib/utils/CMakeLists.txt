add_library(utils
        utils.cpp
        utils.hpp
        ErrorOutput.hpp
)

target_include_directories(utils PUBLIC ${PROJECT_SOURCE_DIR})
