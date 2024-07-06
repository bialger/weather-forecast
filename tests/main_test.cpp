#include <sstream>

#include <gtest/gtest.h>
#include "WeatherForecastIntegrationTestSuite.hpp"
#include "test_functions.hpp"
#include "lib/ui/TextUserInterface.hpp"

TEST_F(WeatherForecastIntegrationTestSuite, HelpTest) {
  std::ostringstream out;
  std::ostringstream err;
  std::istringstream in;
  TextUserInterface tui(out, err, in);
  ASSERT_EQ(tui.Start(SplitString("test --help")), 0);
}

TEST_F(WeatherForecastIntegrationTestSuite, HelpOutputTest) {
  std::ostringstream out;
  std::ostringstream err;
  std::istringstream in;
  TextUserInterface tui(out, err, in);
  tui.Start(SplitString("test --help"));
  ASSERT_EQ(out.str(), kExpectedHelp);
}

TEST_F(WeatherForecastIntegrationTestSuite, NegativeTest1) {
  std::ostringstream out;
  std::ostringstream err;
  std::istringstream in;
  TextUserInterface tui(out, err, in);
  ASSERT_EQ(tui.Start(SplitString("test -c wrong.json")), 1);
}

TEST_F(WeatherForecastIntegrationTestSuite, NegitiveOutputTest1) {
  std::ostringstream out;
  std::ostringstream err;
  std::istringstream in;
  TextUserInterface tui(out, err, in);
  tui.Start(SplitString("test -c wrong.json"));
  ASSERT_EQ(err.str(), "An incorrect value was passed to the --config argument.\n");
  ASSERT_EQ(out.str(), kExpectedHelp);
}
