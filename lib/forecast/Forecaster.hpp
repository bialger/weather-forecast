#ifndef FORECASTER_HPP_
#define FORECASTER_HPP_

#include "JsonCache.hpp"
#include "WeatherDay.hpp"
#include "Geocoder.hpp"
#include "lib/utils/utils.hpp"

class Forecaster {
 public:
  static constexpr int32_t kLowerLimitIntervalSize = 0;
  static constexpr int32_t kUpperLimitIntervalSize = 49;
  static constexpr int32_t kLowerLimitDaysCount = 2;
  static constexpr int32_t kUpperLimitDaysCount = WeatherDay::kDaysInForecast + 1;
  static const std::string kDefaultLocation;

  Forecaster(int32_t interval,
             int32_t days_count,
             const std::string& location,
             const std::string& config_dir,
             const std::string& str_config,
             int32_t& result,
             ErrorOutput error_output);

  static bool IsValidConfig(const std::string& str_config);

  int32_t ObtainForecast();
  int32_t SwapToNext();
  int32_t SwapToPrev();
  int32_t AddDay();
  int32_t RemoveDay();

  [[nodiscard]] std::vector<WeatherDay> GetForecast() const;
  [[nodiscard]] WeatherTimeUnit GetCurrentWeather() const;
  [[nodiscard]] std::string GetLocation() const;
  [[nodiscard]] std::string GetLastForecastTime() const;
  [[nodiscard]] int32_t GetSleepInterval() const;
  [[nodiscard]] bool IsValid() const;

 private:
  std::vector<std::string> locations_{};
  int32_t interval_;
  int32_t days_count_;
  int32_t location_index_;
  bool is_valid_;
  std::string config_dir_;
  std::string last_time_;
  Geocoder geocoder_{};
  std::vector<WeatherDay> forecast_{};
  WeatherTimeUnit current_weather_;
  JsonCache geocoder_cache_;
  ErrorOutput error_output_;

  int32_t AddConfig(const std::string& str_config);

  int32_t RequestPosition();
  int32_t RequestForecast();

  int32_t ProcessPosition(const json& answer);
  int32_t ProcessForecast(const json& answer);
};

#endif //FORECASTER_HPP_
