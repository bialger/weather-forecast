#ifndef WEATHERTIMEUNIT_H_
#define WEATHERTIMEUNIT_H_

#include <string>
#include <cstdint>
#include <map>

#include <nlohmann/json.hpp>

struct WeatherCharName {
  std::string time;
  std::string weather_code;
  std::string temperature;
  std::string apparent_temperature;
  std::string wind_speed;
  std::string visibility;
  std::string pressure;
  std::string precipitation;
  std::string uv_index;
  std::string humidity;
  std::string dates;
};

class WeatherTimeUnit {
 public:
  static const std::map<std::string, std::string> kChargeUnits;
  static const WeatherCharName kShownNames;
  static const std::map<int32_t, std::string> kWeatherCodeToString;

  int32_t weather_type;
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

  [[nodiscard]] std::map<std::string, std::string> GetAllAsMap() const;

 private:
  std::string name_;
};

#endif //WEATHERTIMEUNIT_H_
