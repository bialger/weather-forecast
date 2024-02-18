cmake_minimum_required (VERSION 3.11)

include(FetchContent)

set(FETCHCONTENT_UPDATES_DISCONNECTED TRUE)

FetchContent_Declare(ftxui
        GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui
        GIT_TAG main # Important: Specify a version or a commit hash here.
)

FetchContent_MakeAvailable(ftxui)

add_library(ftxui_interface STATIC
        ftxui_interface.cpp
)

target_link_libraries(ftxui_interface PUBLIC
        ftxui::screen
        ftxui::dom
        ftxui::component
)

target_include_directories(ftxui_interface PUBLIC ${PROJECT_SOURCE_DIR})
