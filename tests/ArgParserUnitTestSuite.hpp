#ifndef ARGPARSERUNITTESTSUITE_HPP_
#define ARGPARSERUNITTESTSUITE_HPP_

#include <filesystem>
#include <string>

#include <gtest/gtest.h>
#include <argparser/basic/BasicFunctions.hpp>

struct ArgParserUnitTestSuite : public testing::Test { // special test structure
  const std::string kTemporaryDirectoryName = (ArgumentParser::IsWindows() ? "" : "./") + std::string("gtest_argparser_tmp");
  const std::string kTemporaryFileName = kTemporaryDirectoryName + (ArgumentParser::IsWindows() ? "\\" : "/") + "temp_file";

  void SetUp() override; // method that is called at the beginning of every test

  void TearDown() override; // method that is called at the end of every test
};

#endif //ARGPARSERUNITTESTSUITE_HPP_
