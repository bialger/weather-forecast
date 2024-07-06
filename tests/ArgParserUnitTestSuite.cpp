#include <fstream>

#include "ArgParserUnitTestSuite.hpp"

void ArgParserUnitTestSuite::SetUp() {
  std::filesystem::create_directories(kTemporaryDirectoryName);
  std::ofstream{kTemporaryFileName.c_str()};
}

void ArgParserUnitTestSuite::TearDown() {
  std::filesystem::remove_all(kTemporaryDirectoryName);
}
