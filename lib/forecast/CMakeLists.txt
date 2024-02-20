add_library(forecast STATIC
        Forecaster.cpp
        Forecaster.hpp
        JsonCache.cpp
        JsonCache.hpp
        WeatherDay.cpp
        WeatherDay.hpp
        WeatherTimeUnit.cpp
        WeatherTimeUnit.hpp
        Geocoder.cpp
        Geocoder.hpp
)

target_link_libraries(forecast PUBLIC
        cpr::cpr
        nlohmann_json::nlohmann_json
        utils
)

target_include_directories(forecast PUBLIC ${PROJECT_SOURCE_DIR})
