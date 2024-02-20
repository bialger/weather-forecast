#include "Forecaster.hpp"
#include "JsonCache.hpp"

#include <cpr/cpr.h>

const std::string Forecaster::kDefaultLocation = "First location in config";

const WeatherCharName Forecaster::kOpenMeteoNames = {
    .time =  "time",
    .weather_code = "weathercode",
    .temperature = "temperature_2m",
    .apparent_temperature = "apparent_temperature",
    .wind_speed = "wind_speed",
    .visibility = "visibility",
    .pressure = "pressure",
    .precipitation = "precipitation",
    .uv_index = "uv_index",
    .humidity = "humidity",
    .dates = "dates"
};
