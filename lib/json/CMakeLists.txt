include(FetchContent)

# Optional: set this to ON if your target publicly links to nlohmann_json and needs to install()
set(JSON_Install ON)

FetchContent_Declare(json
        GIT_REPOSITORY https://github.com/ArthurSonzogni/nlohmann_json_cmake_fetchcontent
        GIT_PROGRESS TRUE  GIT_SHALLOW TRUE
        GIT_TAG v3.11.2
)

FetchContent_MakeAvailable(json)
