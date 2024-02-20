#include "WeatherTimeUnit.hpp"

const WeatherCharName WeatherTimeUnit::kShownNames = {
    .time = "Time",
    .weather_code = "Weather",
    .temperature = "Temperature",
    .apparent_temperature = "Feels like",
    .wind_speed = "Wind",
    .visibility = "Visibility",
    .pressure = "Pressure",
    .precipitation = "Precipitation",
    .uv_index = "UV index",
    .humidity = "Humidity",
    .dates = "Dates"
};

const std::map<std::string, std::string> WeatherTimeUnit::kChargeUnits = {
    {kShownNames.temperature, "°C"},
    {kShownNames.wind_speed, "km/h"},
    {kShownNames.visibility, "km"},
    {kShownNames.pressure, "hPa"},
    {kShownNames.precipitation, "mm"},
    {kShownNames.humidity, "%"}
};

WeatherTimeUnit::WeatherTimeUnit(const std::string& name) : name_(name) {}

std::map<std::string, std::string> WeatherTimeUnit::GetAllAsMap() const {
  std::map<std::string, std::string> result;
  std::string uv_level;

  if (uv_index < 3) {
    uv_level = "Low";
  } else if (uv_index >= 3 && uv_index < 6) {
    uv_level = "Moderate";
  } else if (uv_index >= 6 && uv_index < 8) {
    uv_level = "High";
  } else if (uv_index >= 8 && uv_index < 11) {
    uv_level = "Very high";
  }

  std::string str_visibility = std::string(128, '\0');
  std::snprintf(str_visibility.data(), str_visibility.size(), "%.2f", visibility);
  std::string str_pressure = std::string(128, '\0');
  std::snprintf(str_pressure.data(), str_pressure.size(), "%.1f", pressure / 1000);
  std::string str_uv_index = std::string(128, '\0');
  std::snprintf(str_uv_index.data(), str_uv_index.size(), "%.2f", uv_index);
  std::string str_precipitation = std::string(128, '\0');
  std::snprintf(str_precipitation.data(), str_precipitation.size(), "%.1f", precipitation);

  result[kShownNames.weather_code] = weather_type;
  result[kShownNames.temperature] = std::to_string(real_temperature) + " (" +
      std::to_string(felt_temperature) + ") " + kChargeUnits.at(kShownNames.temperature);
  result[kShownNames.wind_speed] = std::to_string(wind_speed_lower) + " - " +
      std::to_string(wind_speed_upper) + " " + kChargeUnits.at(kShownNames.wind_speed);
  result[kShownNames.visibility] = str_visibility + " " + kChargeUnits.at(kShownNames.visibility);
  result[kShownNames.pressure] = str_pressure + " " + kChargeUnits.at(kShownNames.pressure);
  result[kShownNames.precipitation] = str_precipitation + " " + kChargeUnits.at(kShownNames.precipitation);
  result[kShownNames.uv_index] = str_uv_index + " (" + uv_level + ")";
  result[kShownNames.humidity] = std::to_string(humidity) + " " + kChargeUnits.at(kShownNames.humidity);

  return result;
}
