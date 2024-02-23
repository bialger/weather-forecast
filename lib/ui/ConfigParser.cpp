#include <regex>
#include <fstream>
#include <filesystem>

#include "ConfigParser.hpp"

#include "lib/utils/utils.hpp"

const std::string ConfigParser::kDefaultLocation = "First location in config";

ConfigParser::ConfigParser(int32_t interval,
                           int32_t days_count,
                           const std::string& location,
                           const std::string& config_path,
                           ErrorOutput error_output) : error_output_(error_output) {
  interval_ = interval;
  days_count_ = days_count;
  config_path_ = config_path;
  location_index_ = 0;
  config_dir_ = std::filesystem::absolute(std::filesystem::path(config_path_.c_str())).parent_path().string();

  if (location != kDefaultLocation) {
    locations_.push_back(location);
  }
}

bool ConfigParser::IsValidConfig(const std::string& str_config) {
  json config = json::parse(str_config);

  if (!config.is_object() || !config.contains("api_key_file") || !config.contains("locations")
      || !config.contains("defaults")) {
    return false;
  }

  json api_key_file = config["api_key_file"];
  json locations = config["locations"];
  json defaults = config["defaults"];

  if (!api_key_file.is_string() || !locations.is_array() || !defaults.is_object()) {
    return false;
  }

  if (!defaults.contains("interval") || !defaults.contains("days_count") || !defaults.contains("location_index")) {
    return false;
  }

  return true;
}

int32_t ConfigParser::ParseConfig() {
  std::string str_config = GetStringFromFile(config_path_);

  if (!IsValidConfig(str_config)) {
    DisplayError("Invalid config!\n", error_output_);
    return 1;
  }

  json config = json::parse(str_config);
  json api_key_file = config["api_key_file"];
  json locations = config["locations"];
  json defaults = config["defaults"];

  std::string api_key_path = config_dir_ + "/" + api_key_file.get<std::string>();

  if (!IsRegularFile(api_key_path)) {
    DisplayError("API key file " + api_key_path + " does not exist!\n", error_output_);
    return 1;
  }

  int32_t result = SetApiKey(api_key_path);

  if (result != 0) {
    DisplayError("Invalid API key!\n", error_output_);
    return result;
  }

  if (locations_.empty()) {
    location_index_ = defaults["location_index"].get<int32_t>();
  }

  if (interval_ == kLowerLimitIntervalSize) {
    interval_ = defaults["interval"].get<int32_t>();
  }

  if (days_count_ == Forecaster::kLowerLimitDaysCount) {
    days_count_ = defaults["days_count"].get<int32_t>();
  }

  size_t listed_locations_size = locations.size();
  interval_ = std::clamp(interval_, kLowerLimitIntervalSize + 1, kUpperLimitIntervalSize - 1);
  days_count_ = std::clamp(days_count_, Forecaster::kLowerLimitDaysCount + 1, Forecaster::kUpperLimitDaysCount - 1);
  location_index_ = std::clamp(location_index_, 0, static_cast<int32_t>(listed_locations_size + locations_.size() - 1));

  for (size_t i = 0; i < listed_locations_size; ++i) {
    locations_.push_back(locations[i].get<std::string>());
  }

  return 0;
}

int32_t ConfigParser::SetApiKey(const std::string& api_key_path) {
  std::ifstream api_key_file_(api_key_path);
  std::string api_key = std::string((std::istreambuf_iterator<char>(api_key_file_)),
                                    std::istreambuf_iterator<char>());

  if (api_key.size() < 36) {
    return 1;
  }

  api_key = api_key.substr(0, 36);
  std::regex yandex_api_regex = std::regex(R"(^[0-9a-f]{8}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{12}$)");

  if (!std::regex_match(api_key, yandex_api_regex)) {
    return 1;
  }

  api_key_ = api_key;

  return 0;
}

std::string ConfigParser::GetApiKey() const {
  return api_key_;
}

std::string ConfigParser::GetConfigDir() const {
  return config_dir_;
}

int32_t ConfigParser::GetInterval() const {
  return interval_;
}

int32_t ConfigParser::GetDaysCount() const {
  return days_count_;
}

int32_t ConfigParser::GetLocationIndex() const {
  return location_index_;
}

std::vector<std::string> ConfigParser::GetLocations() const {
  return locations_;
}
