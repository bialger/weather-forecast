add_library(forecast STATIC
        Forecaster.cpp
        Forecaster.hpp
)

target_link_libraries(forecast PUBLIC
        weather
        cpr::cpr
)

target_include_directories(forecast PUBLIC ${PROJECT_SOURCE_DIR})
