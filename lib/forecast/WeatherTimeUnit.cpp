#include "WeatherTimeUnit.hpp"

const std::map<std::string, std::string>WeatherTimeUnit::kChargeUnits = {
    {"Temperature", "°C"},
    {"Wind speed", "km/h"},
    {"Visibility", "km"},
    {"Pressure", "hPa"},
    {"Precipitation", "mm"},
    {"Humidity", "%"}
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

  result["Weather condition"] = weather_type;
  result["Temperature"] = std::to_string(real_temperature) + " (" + std::to_string(felt_temperature) + ") "
      + kChargeUnits.at("Temperature");
  result["Wind speed"] = std::to_string(wind_speed_lower) + " - " +
      std::to_string(wind_speed_upper) + " " + kChargeUnits.at("Wind speed");
  result["Visibility"] = str_visibility + " " + kChargeUnits.at("Visibility");
  result["Pressure"] = str_pressure + " " + kChargeUnits.at("Pressure");
  result["Precipitation"] = str_precipitation + " " + kChargeUnits.at("Precipitation");
  result["UV index"] = str_uv_index + " (" + uv_level + ")";
  result["Humidity"] = std::to_string(humidity) + " " + kChargeUnits.at("Humidity");

  return result;
}
