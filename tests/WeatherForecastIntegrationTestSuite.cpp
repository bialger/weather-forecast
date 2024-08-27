#include "WeatherForecastIntegrationTestSuite.hpp"

const std::string WeatherForecastIntegrationTestSuite::kExpectedHelp = "weather-forecast 1.3.5 Ferdinando II de' Medici\n"
                                  "A program for displaying the weather forecast in the terminal.\n"
                                  "\n"
                                  "OPTIONS:\n"
                                  "-l,  --location=<string>:  Name of the location for which the forecast is requested [default = First location in config]\n"
                                  "-c,  --config=<CompositeString>:  Path to the configuration file [default = default_config.json]\n"
                                  "-L,  --log-file=<CompositeString>:  Path to the log file. If not specified, prints to standard output [default = Print to standard output]\n"
                                  "-d,  --days-count=<int>:  Initial number of days for which the forecast is shown. "
                                  "Should be more than 2 and less than 16. The default value means using the parameter "
                                  "from the configuration file. [default = 2]\n"
                                  "-i,  --interval=<int>:  Initial time between weather forecast updates in hours. "
                                  "Should be more than 0 and less than 49. The default value means using the parameter "
                                  "from the configuration file. [default = 0]\n"
                                  "-v,  --verbose:  Show additional information.\n"
                                  "\n"
                                  "-h,  --help:  Display this help and exit\n";

void WeatherForecastIntegrationTestSuite::SetUp() {
  std::filesystem::create_directories(kTemporaryDirectoryName);
}

void WeatherForecastIntegrationTestSuite::TearDown() {
  std::filesystem::remove_all(kTemporaryDirectoryName);
}
