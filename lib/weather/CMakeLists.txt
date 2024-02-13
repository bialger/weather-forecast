add_library(weather STATIC
        WeatherTimeUnit.cpp
        WeatherTimeUnit.hpp
        WeatherDay.cpp
        WeatherDay.hpp
)

target_link_libraries(weather PRIVATE nlohmann_json::nlohmann_json)

target_include_directories(weather PUBLIC ${PROJECT_SOURCE_DIR})
