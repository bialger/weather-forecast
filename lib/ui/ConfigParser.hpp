#ifndef CONFIG_HPP_
#define CONFIG_HPP_

#include <cstdint>
#include <string>
#include <vector>

#include "lib/utils/utils.hpp"
#include "lib/forecast/Forecaster.hpp"

#include <nlohmann/json.hpp>

using json = nlohmann::json;

class ConfigParser {
 public:
  static constexpr int32_t kLowerLimitIntervalSize = 0;
  static constexpr int32_t kUpperLimitIntervalSize = 49;
  static const std::string kDefaultLocation;

  ConfigParser() = delete;
  ConfigParser(int32_t interval,
               int32_t days_count,
               const std::string& location,
               const std::string& config_path,
               ConditionalOutput error_output);

  static bool IsValidConfig(const std::string& str_config);
  int32_t ParseConfig();

  [[nodiscard]] std::string GetApiKey() const;
  [[nodiscard]] std::string GetConfigDir() const;
  [[nodiscard]] int32_t GetInterval() const;
  [[nodiscard]] int32_t GetDaysCount() const;
  [[nodiscard]] int32_t GetLocationIndex() const;
  [[nodiscard]] std::vector<std::string> GetLocations() const;

 private:
  int32_t interval_;
  int32_t days_count_;
  int32_t location_index_;
  ConditionalOutput error_output_;
  std::string config_dir_;
  std::vector<std::string> locations_{};
  std::string api_key_;
  std::string config_path_;

  int32_t SetApiKey(const std::string& api_key_path);
};

#endif //CONFIG_HPP_
