#include <regex>

#include "Forecaster.hpp"

#include <cpr/cpr.h>

const std::string Forecaster::kDefaultLocation = "First location in config";
Forecaster::Forecaster(int32_t interval,
                       int32_t days_count,
                       const std::string& location,
                       const std::string& config_dir,
                       const std::string& str_config,
                       int32_t& result,
                       ErrorOutput error_output) : geocoder_cache_("geocoder", config_dir),
                                                   config_dir_(config_dir),
                                                   error_output_(error_output),
                                                   current_weather_("Now") {
  interval_ = interval;
  days_count_ = days_count;
  location_index_ = 0;
  forecast_ = std::vector<WeatherDay>(WeatherDay::kDaysInForecast);

  if (location != kDefaultLocation) {
    locations_.push_back(location);
  }

  is_valid_ = false;
  result = AddConfig(str_config);
}

bool Forecaster::IsValidConfig(const std::string& str_config) {
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

int32_t Forecaster::AddConfig(const std::string& str_config) {
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

  int32_t result = geocoder_.SetApiKey(api_key_path);

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

  if (days_count_ == kLowerLimitDaysCount) {
    days_count_ = defaults["days_count"].get<int32_t>();
  }

  size_t listed_locations_size = locations.size();
  std::clamp(location_index_, 0, static_cast<int32_t>(listed_locations_size + locations_.size() - 1));
  is_valid_ = true;

  for (size_t i = 0; i < listed_locations_size; ++i) {
    locations_.push_back(locations[i].get<std::string>());
  }

  return 0;
}

int32_t Forecaster::ObtainForecast() {
  int32_t result = RequestPosition();

  if (result != 0) {
    return 1;
  }

  result = RequestForecast();

  if (result != 0) {
    return 1;
  }

  return 0;
}

int32_t Forecaster::SwapToNext() {
  ++location_index_;

  if (location_index_ == locations_.size()) {
    location_index_ = 0;
  }

  return ObtainForecast();
}

int32_t Forecaster::SwapToPrev() {
  --location_index_;

  if (location_index_ == -1) {
    location_index_ = static_cast<int32_t>(locations_.size()) - 1;
  }

  return ObtainForecast();
}

int32_t Forecaster::AddDay() {
  ++days_count_;

  if (days_count_ == kUpperLimitDaysCount) {
    --days_count_;
  }

  return 0;
}

int32_t Forecaster::RemoveDay() {
  --days_count_;

  if (days_count_ == kLowerLimitDaysCount) {
    ++days_count_;
  }

  return 0;
}

std::vector<WeatherDay> Forecaster::GetForecast() const {
  return {forecast_.begin(), forecast_.begin() + days_count_};
}

WeatherTimeUnit Forecaster::GetCurrentWeather() const {
  return current_weather_;
}

std::string Forecaster::GetLocation() const {
  return locations_[location_index_];
}

std::string Forecaster::GetLastForecastTime() const {
  return last_time_;
}

int32_t Forecaster::GetSleepInterval() const {
  return interval_;
}

bool Forecaster::IsValid() const {
  return is_valid_;
}

int32_t Forecaster::RequestPosition() {
  json answer = geocoder_cache_.GetJsonFromCache(locations_[location_index_]);

  if (answer != JsonCache::kNotFound) {
    return ProcessPosition(answer);
  }

  cpr::Response city_response = cpr::Get(cpr::Url(Geocoder::kGeocoderUrl),
                                         cpr::Parameters{
                                             {"geocode", locations_[location_index_]},
                                             {"apikey", geocoder_.GetApiKey()},
                                             {"format", "json"}
                                         });

  if (city_response.status_code != 200) {
    DisplayError(
        "Invalid response! Make sure that you entered valid Yandex Geocoder API key and it did not reach the limit.\n",
        error_output_);
    return static_cast<int32_t>(city_response.status_code);
  }

  answer = json::parse(city_response.text);

  return ProcessPosition(answer);
}

int32_t Forecaster::RequestForecast() {
  json answer{};

  std::string current_list = WeatherDay::kOpenMeteoNames.temperature + "," + WeatherDay::kOpenMeteoNames.humidity + ","
      + WeatherDay::kOpenMeteoNames.apparent_temperature + "," + WeatherDay::kOpenMeteoNames.precipitation + ","
      + WeatherDay::kOpenMeteoNames.weather_code + "," + WeatherDay::kOpenMeteoNames.pressure + ","
      + WeatherDay::kOpenMeteoNames.wind_speed;
  std::string hourly_list = WeatherDay::kOpenMeteoNames.temperature + "," + WeatherDay::kOpenMeteoNames.humidity + ","
      + WeatherDay::kOpenMeteoNames.apparent_temperature + "," + WeatherDay::kOpenMeteoNames.precipitation + ","
      + WeatherDay::kOpenMeteoNames.weather_code + "," + WeatherDay::kOpenMeteoNames.pressure + ","
      + WeatherDay::kOpenMeteoNames.visibility + "," + WeatherDay::kOpenMeteoNames.wind_speed;
  std::string daily_list = WeatherDay::kOpenMeteoNames.uv_index;

  cpr::Response open_meteo_response = cpr::Get(cpr::Url(WeatherDay::kWeatherUrl),
                                         cpr::Parameters{
                                             {"latitude", geocoder_.GetLatitude()},
                                             {"longitude", geocoder_.GetLongitude()},
                                             {"current", current_list},
                                             {"hourly", hourly_list},
                                             {"daily", daily_list},
                                             {"timezone", "auto"},
                                             {"forecast_days", "16"}
                                         });

  if (open_meteo_response.status_code != 200) {
    DisplayError(
        "Invalid response! Make sure that you entered valid Yandex Geocoder API key and it did not reach the limit.\n",
        error_output_);
    return static_cast<int32_t>(open_meteo_response.status_code);
  }

  answer = json::parse(open_meteo_response.text);

  return ProcessForecast(answer);
}

int32_t Forecaster::ProcessPosition(const json& answer) {
  int32_t result = geocoder_.SetCoordinates(answer);

  if (result != 0) {
    DisplayError("Unknown error occurred while geocoding.\n", error_output_);
    return result;
  }

  geocoder_cache_.PutJsonToCache(locations_[location_index_], answer);

  return 0;
}

int32_t Forecaster::ProcessForecast(const json& answer) {
  for (int32_t day_number = 0; day_number < WeatherDay::kDaysInForecast; ++day_number) {
    bool result = forecast_[day_number].SetForecast(answer, day_number);

    if (!result) {
      DisplayError("Error while processing forecast.\n", error_output_);
      return 1;
    }
  }

  last_time_ = answer["current"]["time"].get<std::string>();

  for (char& letter : last_time_) {
    if (letter == 'T') {
      letter = ' ';
    }
  }

  current_weather_ = WeatherDay::GetCurrentWeather(answer);

  return 0;
}
