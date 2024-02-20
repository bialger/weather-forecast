#include <sstream>

#include <gtest/gtest.h>
#include "WeatherForecastIntegrationTestSuite.hpp"
#include "test_functions.hpp"
#include "lib/ui/TextUserInterface.hpp"

const std::string kExpectedHelp = "weather-forecast\n"
                                  "A program for displaying the weather forecast in the terminal.\n"
                                  "\n"
                                  "OPTIONS:\n"
                                  "-l,  --location=<string>:  Name of the location for which the forecast is requested [default = First location in config]\n"
                                  "-c,  --config=<CompositeString>:  Path to the configuration file [default = default_config.json]\n"
                                  "-d,  --days-count=<int>:  Initial number of days for which the forecast is shown. "
                                  "Should be more than 0 and less than 16. The default value means using the parameter "
                                  "from the configuration file. [default = 0]\n"
                                  "-i,  --interval=<int>:  Initial time between weather forecast updates in hours. "
                                  "Should be more than 0 and less than 49. The default value means using the parameter "
                                  "from the configuration file. [default = 0]\n"
                                  "\n"
                                  "-h,  --help:  Display this help and exit\n";

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
