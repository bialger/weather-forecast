#ifndef WEATHERTIMEUNIT_H_
#define WEATHERTIMEUNIT_H_

#include <string>
#include <map>

#include <nlohmann/json.hpp>

class WeatherTimeUnit {
 public:
  static const std::map<std::string, std::string> kChargeUnits;

  std::string weather_type;
  int32_t real_temperature{};
  int32_t felt_temperature{};
  int32_t wind_speed_lower{};
  int32_t wind_speed_upper{};
  double visibility{};
  double pressure{};
  double precipitation{};
  double uv_index{};
  int32_t humidity{};

  explicit WeatherTimeUnit(const std::string& name);

  std::map<std::string, std::string> GetAllAsMap() const;

 private:
  std::string name_;
};

#endif //WEATHERTIMEUNIT_H_
