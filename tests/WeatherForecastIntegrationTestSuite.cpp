#include "WeatherForecastIntegrationTestSuite.hpp"

void WeatherForecastIntegrationTestSuite::SetUp() {
  std::filesystem::create_directories(kTemporaryDirectoryName);
}

void WeatherForecastIntegrationTestSuite::TearDown() {
  std::filesystem::remove_all(kTemporaryDirectoryName);
}
