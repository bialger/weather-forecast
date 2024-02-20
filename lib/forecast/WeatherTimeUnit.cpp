#include "WeatherTimeUnit.hpp"

const std::map<int32_t, std::string> WeatherTimeUnit::kWeatherCodeToString = {
    {0, "Clear sky"},
    {1, "Mainly clear"},
    {2, "Partly cloudy"},
    {3, "Overcast"},
    {45, "Fog"},
    {48, "Depositing rime fog"},
    {51, "Light drizzle"},
    {53, "Medium drizzle"},
    {55, "Hard drizzle"},
    {56, "Light freezing drizzle"},
    {57, "Hard freezing drizzle"},
    {61, "Light rain"},
    {63, "Medium rain"},
    {65, "Hard rain"},
    {66, "Light freezing rain"},
    {67, "Hard freezing rain"},
    {71, "Light snow fall"},
    {73, "Medium snow fall"},
    {75, "Hard snow fall"},
    {77, "Snow grains"},
    {80, "Light showering"},
    {81, "Medium showering"},
    {82, "Hard showering"},
    {85, "Light snow shower"},
    {86, "Hard snow shower"},
    {95, "Light thunderstorm"},
    {96, "Medium thunderstorm"},
    {99, "Hard thunderstorm"}
};

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
  std::snprintf(str_pressure.data(), str_pressure.size(), "%.1f", pressure);
  std::string str_uv_index = std::string(128, '\0');
  std::snprintf(str_uv_index.data(), str_uv_index.size(), "%.2f", uv_index);
  std::string str_precipitation = std::string(128, '\0');
  std::snprintf(str_precipitation.data(), str_precipitation.size(), "%.1f", precipitation);

  result[kShownNames.weather_code] = kWeatherCodeToString.at(weather_type);
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
