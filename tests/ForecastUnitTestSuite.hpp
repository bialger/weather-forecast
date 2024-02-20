#ifndef FORECASTUNITTESTSUITE_HPP_
#define FORECASTUNITTESTSUITE_HPP_

#include "lib/utils/utils.hpp"

#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

struct ForecastUnitTestSuite : public testing::Test { // special test structure
  const std::string kTemporaryDirectoryName = (IsWindows() ? "" : "./") + std::string("gtest_forecast_tmp");
  const std::string kTemporaryFileName = kTemporaryDirectoryName + (IsWindows() ? "\\" : "/") + "sample.json";
  const json kSampleData = R"({"string":"sample","int":1,"bool":true,"arr":["arr0","arr1"],"nested":{"string":"str"}})";

  void SetUp() override; // method that is called at the beginning of every test

  void TearDown() override; // method that is called at the end of every test
};

#endif //FORECASTUNITTESTSUITE_HPP_
