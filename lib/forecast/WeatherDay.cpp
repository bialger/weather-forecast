#include <cmath>

#include "WeatherDay.hpp"
#include "lib/utils/utils.hpp"

const WeatherCharName WeatherDay::kOpenMeteoNames = {
    .time =  "time",
    .weather_code = "weathercode",
    .temperature = "temperature_2m",
    .apparent_temperature = "apparent_temperature",
    .wind_speed = "wind_speed_10m",
    .visibility = "visibility",
    .pressure = "pressure_msl",
    .precipitation = "precipitation",
    .uv_index = "uv_index_max",
    .humidity = "relative_humidity_2m",
    .dates = "time"
};

WeatherDay::WeatherDay() {
  hourly_values_ = {
      {kOpenMeteoNames.time, {}},
      {kOpenMeteoNames.weather_code, {}},
      {kOpenMeteoNames.temperature, {}},
      {kOpenMeteoNames.apparent_temperature, {}},
      {kOpenMeteoNames.wind_speed, {}},
      {kOpenMeteoNames.visibility, {}},
      {kOpenMeteoNames.pressure, {}},
      {kOpenMeteoNames.precipitation, {}},
      {kOpenMeteoNames.humidity, {}}
  };

  daily_values_ = {
      {kOpenMeteoNames.uv_index, {}},
      {kOpenMeteoNames.dates, {}}
  };

  units_.emplace_back("Night");
  units_.emplace_back("Morning");
  units_.emplace_back("Day");
  units_.emplace_back("Evening");
}

const std::string WeatherDay::kWeatherUrl = "http://api.open-meteo.com/v1/forecast";

WeatherTimeUnit WeatherDay::GetCurrentWeather(const json& forecast) {
  WeatherTimeUnit time_unit("Now");

  if (!forecast.is_object() || forecast.contains("error")) {
    return time_unit;
  }

  if (!forecast.contains("hourly") || !forecast.contains("daily") || !forecast.contains("current")) {
    return time_unit;
  }

  const json& hourly = forecast["hourly"];
  const json& daily = forecast["daily"];
  const json& current = forecast["current"];

  if (!hourly.is_object() || !daily.is_object() || !current.is_object()) {
    return time_unit;
  }

  const int32_t hour = std::stoi(current[kOpenMeteoNames.time].get<std::string>().substr(11, 2));

  if (hour < 0 || hour >= kHoursInDay) {
    return time_unit;
  }

  time_unit.weather_type = current[kOpenMeteoNames.weather_code].get<int32_t>();
  time_unit.real_temperature = static_cast<int32_t>(std::round(current[kOpenMeteoNames.temperature].get<double>()));
  time_unit.felt_temperature =
      static_cast<int32_t>(std::round(current[kOpenMeteoNames.apparent_temperature].get<double>()));
  time_unit.wind_speed_lower = static_cast<int32_t>(std::round(current[kOpenMeteoNames.wind_speed].get<double>()));
  time_unit.wind_speed_upper = time_unit.wind_speed_lower;
  time_unit.visibility = hourly[kOpenMeteoNames.visibility][hour].get<double>() / 1000;
  time_unit.pressure = current[kOpenMeteoNames.pressure].get<double>();
  time_unit.precipitation = current[kOpenMeteoNames.precipitation].get<double>();
  time_unit.uv_index = daily[kOpenMeteoNames.uv_index][0].get<double>();
  time_unit.humidity = current[kOpenMeteoNames.humidity].get<int32_t>();

  return time_unit;
}

bool WeatherDay::SetForecast(const json& forecast, int32_t day_number) {
  if (!SetJsonProperties(forecast)) {
    return false;
  }

  date_ = daily_values_[kOpenMeteoNames.dates][day_number].get<std::string>();
  const double uv_index = daily_values_[kOpenMeteoNames.uv_index][day_number].get<double>();

  const size_t start_index = day_number * kHoursInDay;
  const size_t end_index = start_index + kHoursInDay - 1;
  constexpr size_t kHoursInUnit = kHoursInDay / kUnitsInDay;

  for (size_t unit_index = 0; unit_index < kUnitsInDay; ++unit_index) {
    WeatherTimeUnit& unit = units_[unit_index];
    unit.uv_index = uv_index;

    int32_t biggest_weather_code = std::numeric_limits<int32_t>::min();
    double temperature_summa = 0;
    double felt_temperature_summa = 0;
    double biggest_wind_speed = std::numeric_limits<double>::min();
    double smallest_wind_speed = std::numeric_limits<double>::max();
    double visibility_summa = 0;
    double pressure_summa = 0;
    double precipitation_summa = 0;
    double humidity_summa = 0;

    for (size_t i = 0; i < kHoursInUnit; ++i) {
      const size_t element_index = start_index + unit_index * kHoursInUnit + i;

      if (element_index > end_index) {
        return false;
      }

      int32_t weather_code = 0;

      if (!hourly_values_[kOpenMeteoNames.weather_code][element_index].is_null()) {
        weather_code = hourly_values_[kOpenMeteoNames.weather_code][element_index].get<int32_t>();
      }

      biggest_weather_code = (weather_code > biggest_weather_code) ? weather_code : biggest_weather_code;

      if (!hourly_values_[kOpenMeteoNames.temperature][element_index].is_null()) {
        temperature_summa += hourly_values_[kOpenMeteoNames.temperature][element_index].get<double>();
      }

      if (!hourly_values_[kOpenMeteoNames.apparent_temperature][element_index].is_null()) {
        felt_temperature_summa += hourly_values_[kOpenMeteoNames.apparent_temperature][element_index].get<double>();
      }

      double wind_speed = 0;

      if (!hourly_values_[kOpenMeteoNames.wind_speed][element_index].is_null()) {
        wind_speed = hourly_values_[kOpenMeteoNames.wind_speed][element_index].get<double>();
      }

      biggest_wind_speed = (wind_speed > biggest_wind_speed) ? wind_speed : biggest_wind_speed;
      smallest_wind_speed = (wind_speed < smallest_wind_speed) ? wind_speed : smallest_wind_speed;

      if (!hourly_values_[kOpenMeteoNames.visibility][element_index].is_null()) {
        visibility_summa += hourly_values_[kOpenMeteoNames.visibility][element_index].get<double>();
      }

      if (!hourly_values_[kOpenMeteoNames.pressure][element_index].is_null()) {
        pressure_summa += hourly_values_[kOpenMeteoNames.pressure][element_index].get<double>();
      }

      if (!hourly_values_[kOpenMeteoNames.precipitation][element_index].is_null()) {
        precipitation_summa += hourly_values_[kOpenMeteoNames.precipitation][element_index].get<double>();
      }

      if (!hourly_values_[kOpenMeteoNames.humidity][element_index].is_null()) {
        humidity_summa += hourly_values_[kOpenMeteoNames.humidity][element_index].get<int32_t>();
      }
    }

    unit.weather_type = biggest_weather_code;
    unit.real_temperature = static_cast<int32_t>(std::round(temperature_summa / kHoursInUnit));
    unit.felt_temperature = static_cast<int32_t>(std::round(felt_temperature_summa / kHoursInUnit));
    unit.wind_speed_upper = static_cast<int32_t>(std::round(biggest_wind_speed));
    unit.wind_speed_lower = static_cast<int32_t>(std::round(smallest_wind_speed));
    unit.visibility = visibility_summa / (kHoursInUnit * 1000);
    unit.pressure = pressure_summa / kHoursInUnit;
    unit.precipitation = precipitation_summa / kHoursInUnit;
    unit.humidity = static_cast<int32_t>(std::round(humidity_summa / kHoursInUnit));
  }

  return true;
}

bool WeatherDay::SetJsonProperties(const json& forecast) {
  if (!forecast.is_object() || forecast.contains("error")) {
    return false;
  }

  if (!forecast.contains("hourly") || !forecast.contains("daily")) {
    return false;
  }

  const json& hourly = forecast["hourly"];
  const json& daily = forecast["daily"];

  if (!hourly.is_object() || !daily.is_object()) {
    return false;
  }

  for (auto& [name, hourly_value] : hourly_values_) {
    if (!hourly.contains(name)) {
      return false;
    }

    hourly_value = hourly[name];

    if (!hourly_value.is_array() || hourly_value.size() < kHoursInDay * kDaysInForecast) {
      return false;
    }
  }

  for (auto& [name, daily_value] : daily_values_) {
    if (!daily.contains(name)) {
      return false;
    }

    daily_value = daily[name];

    if (!daily_value.is_array() || daily_value.size() < kDaysInForecast) {
      return false;
    }
  }

  return true;
}

std::string WeatherDay::GetDate() const {
  return date_;
}

std::vector<WeatherTimeUnit> WeatherDay::GetForecastUnits() const {
  return units_;
}
