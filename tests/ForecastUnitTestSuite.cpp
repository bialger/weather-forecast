#include <fstream>
#include <filesystem>

#include "ForecastUnitTestSuite.hpp"

void ForecastUnitTestSuite::SetUp() {
  std::filesystem::create_directories(kTemporaryDirectoryName);
  std::ofstream temp_file{kTemporaryFileName.c_str()};
  temp_file << kSampleData;
}

void ForecastUnitTestSuite::TearDown() {
  std::filesystem::remove_all(kTemporaryDirectoryName);
}
