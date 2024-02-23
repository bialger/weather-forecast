add_library(ui STATIC
        TextUserInterface.cpp
        TextUserInterface.hpp
        TuiWorker.cpp
        TuiWorker.hpp
        ConfigParser.cpp
        ConfigParser.hpp
)

target_link_libraries(ui PUBLIC
        argparser
        forecast
        ftxui_interface
        utils
)

target_include_directories(ui PUBLIC ${PROJECT_SOURCE_DIR})
