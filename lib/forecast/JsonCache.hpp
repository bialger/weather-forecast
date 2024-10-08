#ifndef JSONCACHE_HPP_
#define JSONCACHE_HPP_

#include <string>

#include <nlohmann/json.hpp>

using json = nlohmann::json;

class JsonCache {
 public:
  static const json kNotFound;

  JsonCache() = delete;
  JsonCache(const std::string& cache_group, const std::string& common_cache_dir);

  void PutJsonToCache(const std::string& cache_name, const json& data) const;
  json GetJsonFromCache(const std::string& cache_name) const;

 private:
  std::string cache_group_;
  std::string cache_dir_;

  std::string GetCacheFilename(const std::string& cache_name) const;
};

#endif //JSONCACHE_HPP_
