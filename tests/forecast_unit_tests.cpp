#include <sstream>

#include "lib/forecast/Forecaster.hpp"
#include "test_functions.hpp"
#include "ForecastUnitTestSuite.hpp"

#include <gtest/gtest.h>

TEST_F(ForecastUnitTestSuite, JsonCacheDirTest1) {
  JsonCache cache("tests", kTemporaryDirectoryName);
  ASSERT_TRUE(std::filesystem::is_directory(kTemporaryDirectoryName + "/cache/tests"));
}

TEST_F(ForecastUnitTestSuite, JsonCacheFileTest1) {
  JsonCache cache("tests", kTemporaryDirectoryName);
  cache.PutJsonToCache("sample", kSampleData);
  ASSERT_TRUE(std::filesystem::is_regular_file(kTemporaryDirectoryName + "/cache/tests/sample.json"));
}

TEST_F(ForecastUnitTestSuite, JsonCachePutGetTest1) {
  JsonCache cache("tests", kTemporaryDirectoryName);
  cache.PutJsonToCache("sample", kSampleData);
  json opened = cache.GetJsonFromCache("sample");
  ASSERT_FALSE(JsonCache::kNotFound == opened);
  ASSERT_TRUE(kSampleData == opened);
}

TEST_F(ForecastUnitTestSuite, JsonCacheGetNegativeTest1) {
  JsonCache cache("tests", kTemporaryDirectoryName);
  json opened = cache.GetJsonFromCache("unavailable_cache");
  ASSERT_TRUE(JsonCache::kNotFound == opened);
  ASSERT_FALSE(kSampleData == opened);
}
