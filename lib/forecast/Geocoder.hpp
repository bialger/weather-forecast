#ifndef GEOCODER_HPP_
#define GEOCODER_HPP_

#include <nlohmann/json.hpp>

using json = nlohmann::json;

class Geocoder {
 public:
  static const std::string kGeocoderUrl;

  Geocoder() = default;

  int32_t SetCoordinates(const json& geocode);

  [[nodiscard]] std::string GetLatitude() const;
  [[nodiscard]] std::string GetLongitude() const;

 private:
  std::string latitude_;
  std::string longitude_;
};

#endif //GEOCODER_HPP_
