#ifndef FORECASTER_HPP_
#define FORECASTER_HPP_

#include "JsonCache.hpp"
#include "WeatherDay.hpp"
#include "WeatherTimeUnit.hpp"

class Forecaster {
 public:
  static constexpr int32_t kLowerLimitIntervalSize = 0;
  static constexpr int32_t kUpperLimitIntervalSize = 49;
  static constexpr int32_t kLowerLimitDaysCount = 0;
  static constexpr int32_t kUpperLimitDaysCount = WeatherDay::kDaysInForecast + 1;
  static const std::string kDefaultLocation;
};

#endif //FORECASTER_HPP_
