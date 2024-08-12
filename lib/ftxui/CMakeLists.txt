cmake_minimum_required (VERSION 3.11)

include(FetchContent)

set(FETCHCONTENT_UPDATES_DISCONNECTED TRUE)

FetchContent_Declare(ftxui
        GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui
        GIT_TAG main # Important: Specify a version or a commit hash here.
)

FetchContent_MakeAvailable(ftxui)

add_library(ftxui_interface INTERFACE
        ftxui_interface.hpp
)

target_link_libraries(ftxui_interface INTERFACE
        ftxui::screen
        ftxui::dom
        ftxui::component
)

target_include_directories(ftxui_interface INTERFACE ${PROJECT_SOURCE_DIR})
