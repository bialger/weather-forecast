#include <sstream>

#include <gtest/gtest.h>
#include "WeatherForecastIntegrationTestSuite.hpp"
#include "test_functions.hpp"
#include "lib/ui/ui_functions.hpp"

TEST_F(WeatherForecastIntegrationTestSuite, NegativeTest1) {
  std::ostringstream out;
  ASSERT_EQ(StartConsoleUI(SplitString("test"), out), 1);
}

TEST_F(WeatherForecastIntegrationTestSuite, NegitiveOutputTest1) {
  std::ostringstream out;
  StartConsoleUI(SplitString("test"), out);
  ASSERT_EQ(out.str(), "Insufficient arguments\n");
}
