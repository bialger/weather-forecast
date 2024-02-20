#ifndef WEATHERDAY_HPP_
#define WEATHERDAY_HPP_

#include "WeatherTimeUnit.hpp"

#include <nlohmann/json.hpp>

using json = nlohmann::json;

class WeatherDay {
 public:
  static constexpr int32_t kUnitsInDay = 4;
  static constexpr int32_t kHoursInDay = 24;
  static constexpr int32_t kDaysInForecast = 16;
  static const WeatherCharName kOpenMeteoNames;
  static const std::string kWeatherUrl;

  WeatherDay();

  static WeatherTimeUnit GetCurrentWeather(const json& forecast);

  bool SetForecast(const json& forecast, int32_t day_number);

  [[nodiscard]] std::string GetDate() const;
  [[nodiscard]] std::vector<WeatherTimeUnit> GetForecastUnits() const;

 private:
  std::string date_{};
  std::vector<WeatherTimeUnit> units_{};
  std::map<std::string, json> hourly_values_{};
  std::map<std::string, json> daily_values_{};

  bool SetJsonProperties(const json& forecast);
};

#endif //WEATHERDAY_HPP_
