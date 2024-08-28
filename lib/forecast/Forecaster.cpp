#include <regex>
#include <utility>

#include "Forecaster.hpp"

#include <cpr/cpr.h>

Forecaster::Forecaster(int32_t days_count,
                       int32_t location_index,
                       const std::vector<std::string>& locations,
                       std::string  api_key,
                       const std::string& config_dir,
                       ConditionalOutput error_output,
                       ConditionalOutput log_output) : locations_(locations),
                                                       days_count_(days_count),
                                                       location_index_(location_index),
                                                       api_key_(std::move(api_key)),
                                                       current_weather_("Now"),
                                                       geocoder_cache_("geocoder", config_dir),
                                                       error_output_(error_output),
                                                       log_output_(log_output) {
  forecast_ = std::vector<WeatherDay>(WeatherDay::kDaysInForecast);
}

int32_t Forecaster::ObtainForecast() {
  WriteCurrentTime(log_output_);
  log_output_ << "FORECASTER: Obtaining forecast for " << locations_[location_index_] << "\n";

  int32_t result = RequestPosition();

  if (result != 0) {
    return 1;
  }

  WriteCurrentTime(log_output_);
  log_output_ << "FORECASTER: Coordinates obtained: " << geocoder_.GetLatitude() << " latitude, "
              << geocoder_.GetLongitude() << " longitude\n";

  result = RequestForecast();

  if (result != 0) {
    return 1;
  }

  WriteCurrentTime(log_output_);
  log_output_ << "FORECASTER: Forecast obtained successfully\n";

  return 0;
}

int32_t Forecaster::SwapToNext() {
  WriteCurrentTime(log_output_);
  log_output_ << "FORECASTER: Swapping to next location from " << locations_[location_index_] << " (" << location_index_
              << ") ";

  ++location_index_;

  if (location_index_ == locations_.size()) {
    location_index_ = 0;
  }

  log_output_ << "to " << locations_[location_index_] << " (" << location_index_ << ")\n";

  return ObtainForecast();
}

int32_t Forecaster::SwapToPrev() {
  WriteCurrentTime(log_output_);
  log_output_ << "FORECASTER: Swapping to previous location from " << locations_[location_index_] << " ("
              << location_index_
              << ") ";

  --location_index_;

  if (location_index_ == -1) {
    location_index_ = static_cast<int32_t>(locations_.size()) - 1;
  }

  log_output_ << "to " << locations_[location_index_] << " (" << location_index_ << ")\n";

  return ObtainForecast();
}

int32_t Forecaster::AddDay() {
  WriteCurrentTime(log_output_);
  log_output_ << "FORECASTER: Adding day from " << days_count_ << " ";

  ++days_count_;

  if (days_count_ == kUpperLimitDaysCount) {
    --days_count_;
  }

  log_output_ << "to " << days_count_ << "\n";

  return 0;
}

int32_t Forecaster::RemoveDay() {
  WriteCurrentTime(log_output_);
  log_output_ << "FORECASTER: Removing day from " << days_count_ << " ";

  --days_count_;

  if (days_count_ == kLowerLimitDaysCount) {
    ++days_count_;
  }

  log_output_ << "to " << days_count_ << "\n";

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

ConditionalOutput Forecaster::GetErrorOutput() const {
  return error_output_;
}

ConditionalOutput Forecaster::GetLogOutput() const {
  return log_output_;
}

int32_t Forecaster::RequestPosition() {
  json answer = geocoder_cache_.GetJsonFromCache(locations_[location_index_]);

  if (answer != JsonCache::kNotFound) {
    WriteCurrentTime(log_output_);
    log_output_ << "FORECASTER: Coordinates were obtained from cache\n";

    return ProcessPosition(answer);
  }

  WriteCurrentTime(log_output_);
  log_output_ << "FORECASTER: Obtaining coordinates from Yandex Geocoder\n";

  cpr::Response yandex_geocoder_response = cpr::Get(cpr::Url(Geocoder::kGeocoderUrl),
                                                    cpr::Parameters{
                                                        {"geocode", locations_[location_index_]},
                                                        {"apikey", api_key_},
                                                        {"format", "json"}
                                                    });

  WriteCurrentTime(log_output_);
  log_output_ << "FORECASTER: Yandex Geocoder request: " << yandex_geocoder_response.url << "\n";


  if (!yandex_geocoder_response.text.empty()) {
    answer = json::parse(yandex_geocoder_response.text);
  }

  if (yandex_geocoder_response.status_code != 200) {
    std::string error_message = "Invalid response! ";
    if (answer.contains("reason") && answer["message"].is_string()) {
      error_message += "Yandex Geocoder sent an error. \nError description: " + answer["message"].get<std::string>();
    } else if (yandex_geocoder_response.status_code == 0) {
      error_message += "Yandex Geocoder did not respond. Make sure that you have access to the Internet.";
    } else {
      error_message += "Yandex Geocoder sent an unknown error.";
    }

    DisplayError(error_message + "\n", error_output_);
    return static_cast<int32_t>(yandex_geocoder_response.status_code);
  }

  return ProcessPosition(answer);
}

int32_t Forecaster::RequestForecast() {
  json answer{};

  WriteCurrentTime(log_output_);
  log_output_ << "FORECASTER: Obtaining forecast from Open Meteo\n";

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

  WriteCurrentTime(log_output_);
  log_output_ << "FORECASTER: Open Meteo request: " << open_meteo_response.url << "\n";

  if (!open_meteo_response.text.empty()) {
    answer = json::parse(open_meteo_response.text);
  }

  if (open_meteo_response.status_code != 200) {
    std::string error_message = "Invalid response! ";
    if (answer.contains("reason") && answer["reason"].is_string()) {
      error_message += "OpenMeteo sent an error.\nError description: " + answer["reason"].get<std::string>();
    } else if (open_meteo_response.status_code == 0) {
      WriteCurrentTime(log_output_);
      log_output_ << "FORECASTER: WARNING! OpenMeteo did not respond. Make sure that you have access to the Internet.\n";
      error_message += "OpenMeteo did not respond. Make sure that you have access to the Internet.";
    } else {
      error_message += "OpenMeteo sent an unknown error.";
    }

    DisplayError(error_message + "\n", error_output_);
    return static_cast<int32_t>(open_meteo_response.status_code);
  }

  return ProcessForecast(answer);
}

int32_t Forecaster::ProcessPosition(const json& answer) {
  WriteCurrentTime(log_output_);
  log_output_ << "FORECASTER: Processing position\n";

  const int32_t result = geocoder_.SetCoordinates(answer);

  if (result != 0) {
    DisplayError("Unknown error occurred while geocoding.\n", error_output_);
    return result;
  }

  geocoder_cache_.PutJsonToCache(locations_[location_index_], answer);

  WriteCurrentTime(log_output_);
  log_output_ << "FORECASTER: Geocoder response put in cache for " << locations_[location_index_] << "\n";

  WriteCurrentTime(log_output_);
  log_output_ << "FORECASTER: Position obtained successfully\n";

  return 0;
}

int32_t Forecaster::ProcessForecast(const json& answer) {
  for (int32_t day_number = 0; day_number < WeatherDay::kDaysInForecast; ++day_number) {
    const bool result = forecast_[day_number].SetForecast(answer, day_number);

    if (!result) {
      geocoder_cache_.PutJsonToCache("error", answer);
      DisplayError("Error while processing forecast for day " + std::to_string(day_number + 1) + ".\n", error_output_);
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

  WriteCurrentTime(log_output_);
  log_output_ << "FORECASTER: Forecast processed successfully for time " << last_time_ << "\n";

  return 0;
}
