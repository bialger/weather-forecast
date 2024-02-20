#include <fstream>

#include "JsonCache.hpp"
#include "lib/utils/utils.hpp"
#include "lib/utils/utils.hpp"

const json JsonCache::kNotFound = "{}";

JsonCache::JsonCache(const std::string& cache_group, const std::string& common_cache_dir) {
  cache_group_ = cache_group;
  cache_dir_ = common_cache_dir + "/cache/" + cache_group_;

  if (!std::filesystem::is_directory(cache_dir_)) {
    std::filesystem::create_directories(cache_dir_);
  }
}

void JsonCache::PutJsonToCache(const std::string& cache_name, const json& data) {
  std::ofstream cache_file(GetCacheFilename(cache_name).c_str());
  cache_file << data;
}

json JsonCache::GetJsonFromCache(const std::string& cache_name) {
  if (!std::filesystem::is_regular_file(GetCacheFilename(cache_name))) {
    return kNotFound;
  }

  std::ifstream cache_file(GetCacheFilename(cache_name).c_str());
  json data = json::parse(cache_file);
  return data;
}

std::string JsonCache::GetCacheFilename(const std::string& cache_name) {
  return cache_dir_ + "/" + cache_name + ".json";
}
