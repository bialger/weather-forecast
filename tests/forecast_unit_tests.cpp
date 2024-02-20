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
  ASSERT_TRUE(std::equal(kSampleData.begin(), kSampleData.end(), opened.begin(), opened.end()));
}
