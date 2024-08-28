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

  const std::vector<std::string> positions = Split(
      geocode["response"]["GeoObjectCollection"]["featureMember"][0]["GeoObject"]["Point"]["pos"].get<std::string>()
  );

  longitude_ = positions[0];
  latitude_ = positions[1];

  return 0;
}

std::string Geocoder::GetLatitude() const {
  return latitude_;
}

std::string Geocoder::GetLongitude() const {
  return longitude_;
}
