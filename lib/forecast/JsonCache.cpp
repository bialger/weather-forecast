#include <fstream>

#include "JsonCache.hpp"
#include "lib/utils/utils.hpp"
#include "lib/utils/utils.hpp"

JsonCache::JsonCache(const std::string& cache_group, const std::string& common_cache_dir) {
  cache_group_ = cache_group;
  cache_dir_ = common_cache_dir + "/" + cache_group_;

  if (!std::filesystem::is_directory(cache_dir_)) {
    std::filesystem::create_directories(cache_dir_);
  }
}

void JsonCache::PutJsonToCache(const std::string& cache_name, const json& data) {
  std::ofstream cache_file(GetCacheFilename(cache_name).c_str(), std::ios::out | std::ios::binary);
  std::string json_contents = to_string(data);
  WriteStringToStream(json_contents, cache_file);
}

json JsonCache::GetJsonFromCache(const std::string& cache_name) {
  std::string cache_contents = GetStringFromFile(GetCacheFilename(cache_name));
  json data = json::parse(cache_name);
  return data;
}

std::string JsonCache::GetCacheFilename(const std::string& cache_name) {
  return cache_dir_ + "/" + cache_name + ".json";
}
