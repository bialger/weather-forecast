#include <regex>
#include <fstream>

#include "Geocoder.hpp"
#include "lib/utils/utils.hpp"

const std::string Geocoder::kGeocoderUrl = "http://geocode-maps.yandex.ru/1.x";

int32_t Geocoder::SetCoordinates(const json& geocode) {
  if (!geocode.is_object()
      || !geocode.contains("response")
      || !geocode["response"].contains("GeoObjectCollection")
      || !geocode["response"]["GeoObjectCollection"].contains("featureMember")
      || !geocode["response"]["GeoObjectCollection"]["featureMember"].is_array()
      || !geocode["response"]["GeoObjectCollection"]["featureMember"][0].is_object()
      || !geocode["response"]["GeoObjectCollection"]["featureMember"][0].contains("GeoObject")
      || !geocode["response"]["GeoObjectCollection"]["featureMember"][0]["GeoObject"].contains("Point")
      || !geocode["response"]["GeoObjectCollection"]["featureMember"][0]["GeoObject"]["Point"].contains("pos")
      || !geocode["response"]["GeoObjectCollection"]["featureMember"][0]["GeoObject"]["Point"]["pos"].is_string()) {
    return 1;
  }

  std::vector<std::string> positions = Split(
      geocode["response"]["GeoObjectCollection"]["featureMember"][0]["GeoObject"]["Point"]["pos"].get<std::string>()
  );

  latitude_ = positions[0];
  longitude_ = positions[1];

  return 0;
}

int32_t Geocoder::SetApiKey(const std::string& api_key_path) {
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

std::string Geocoder::GetLatitude() const {
  return latitude_;
}

std::string Geocoder::GetLongitude() const {
  return longitude_;
}

std::string Geocoder::GetApiKey() const {
  return api_key_;
}
