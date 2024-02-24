#ifndef FORECASTER_HPP_
#define FORECASTER_HPP_

#include "JsonCache.hpp"
#include "WeatherDay.hpp"
#include "Geocoder.hpp"

#include "lib/utils/utils.hpp"

class Forecaster {
 public:
  static constexpr int32_t kLowerLimitDaysCount = 2;
  static constexpr int32_t kUpperLimitDaysCount = WeatherDay::kDaysInForecast + 1;

  Forecaster(int32_t days_count,
             int32_t location_index,
             const std::vector<std::string>& locations,
             const std::string& api_key,
             const std::string& config_dir,
             ConditionalOutput error_output,
             ConditionalOutput log_output);

  int32_t ObtainForecast();
  int32_t SwapToNext();
  int32_t SwapToPrev();
  int32_t AddDay();
  int32_t RemoveDay();

  [[nodiscard]] std::vector<WeatherDay> GetForecast() const;
  [[nodiscard]] WeatherTimeUnit GetCurrentWeather() const;
  [[nodiscard]] std::string GetLocation() const;
  [[nodiscard]] std::string GetLastForecastTime() const;
  [[nodiscard]] ConditionalOutput GetErrorOutput() const;
  [[nodiscard]] ConditionalOutput GetLogOutput() const;

 private:
  std::vector<std::string> locations_{};
  int32_t days_count_;
  int32_t location_index_;
  std::string last_time_;
  std::string api_key_;
  Geocoder geocoder_{};
  std::vector<WeatherDay> forecast_{};
  WeatherTimeUnit current_weather_;
  JsonCache geocoder_cache_;
  ConditionalOutput error_output_;
  ConditionalOutput log_output_;

  int32_t RequestPosition();
  int32_t RequestForecast();

  int32_t ProcessPosition(const json& answer);
  int32_t ProcessForecast(const json& answer);
};

#endif //FORECASTER_HPP_
