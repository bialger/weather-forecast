#include <fstream>
#include <iterator>
#include <functional>

#include "ui_functions.hpp"
#include "lib/forecast/Forecaster.hpp"
#include "lib/utils/utils.hpp"
#include "lib/argparser/ArgParser.hpp"
#include "lib/ftxui/ftxui_interface.hpp"

#include <cpr/cpr.h>

int32_t StartConsoleUI(const std::vector<std::string>& args, std::ostream& out, std::istream& in) {
  SetOutputToUnicode();
  ErrorOutput error_output = {out, true};
  ArgumentParser::ArgParser parser("weather-forecast");
  std::vector<std::string> potential_config_dirs = GetPotentialConfigDirectories();
  CompositeString default_config_path = "default_config.json";
  CompositeString config_path;
  std::string interval_description = "Initial time between weather forecast updates in hours. Should be more than "
      + std::to_string(Forecaster::kLowerLimitIntervalSize) + " and less than "
      + std::to_string(Forecaster::kUpperLimitIntervalSize)
      + ". The default value means using the parameter from the configuration file.";
  std::string days_count_description = "Initial number of days for which the forecast is shown. Should be more than "
      + std::to_string(Forecaster::kLowerLimitDaysCount) + " and less than "
      + std::to_string(Forecaster::kUpperLimitDaysCount)
      + ". The default value means using the parameter from the configuration file.";

  std::function<bool(std::string&)> IsGoodIntervalLength = [&](std::string& str_size) -> bool {
    int32_t block_size = std::stoi(str_size);
    return block_size > Forecaster::kLowerLimitIntervalSize && block_size < Forecaster::kUpperLimitIntervalSize;
  };

  std::function<bool(std::string&)> IsGoodDaysCount = [&](std::string& str_count) -> bool {
    int32_t days_count = std::stoi(str_count);
    return days_count > Forecaster::kLowerLimitDaysCount && days_count < Forecaster::kUpperLimitDaysCount;
  };

  parser.AddCompositeArgument('c', "config", "Path to the configuration file").Default(default_config_path)
      .StoreValue(config_path).AddValidate(&IsValidFilename).AddIsGood(&IsRegularFile);
  parser.AddStringArgument('l', "location", "Name of the location for which the forecast is requested")
      .Default(Forecaster::kDefaultLocation);
  parser.AddIntArgument('i',
                        "interval",
                        interval_description.c_str())
      .Default(Forecaster::kLowerLimitIntervalSize).AddIsGood(IsGoodIntervalLength);
  parser.AddIntArgument('d',
                        "days-count",
                        days_count_description.c_str())
      .Default(Forecaster::kLowerLimitDaysCount).AddIsGood(IsGoodDaysCount);
  parser.AddHelp('h', "help", "A program for displaying the weather forecast in the terminal.");

  if (!parser.Parse(args, error_output)) {
    out << parser.HelpDescription();
    return 1;
  }

  if (parser.Help()) {
    out << parser.HelpDescription();
    return 0;
  }

  if (parser.GetCompositeValue("config") == default_config_path) {
    for (const std::string& potential_config_dir : potential_config_dirs) {
      if (std::filesystem::is_regular_file(potential_config_dir + "/" + parser.GetCompositeValue("config"))) {
        config_path = potential_config_dir + "/" + parser.GetCompositeValue("config");
        break;
      }
    }
  }

  if (!IsRegularFile(config_path)) {
    DisplayError("No configuration file was found!\n", error_output);
    out << parser.HelpDescription();
    return 1;
  }

  return BeginForecast(parser.GetIntValue("interval"),
                       parser.GetIntValue("days-count"),
                       parser.GetStringValue("location"),
                       config_path,
                       out,
                       in);
}

int32_t BeginForecast(int32_t interval,
                      int32_t days_count,
                      const std::string& location,
                      const std::string& config_path,
                      std::ostream& out,
                      std::istream& in) {
  std::string config_contents = GetStringFromFile(config_path);
  std::string config_dir = std::filesystem::absolute(std::filesystem::path(config_path)).parent_path().string();

  return 0;
}

std::vector<std::string> GetPotentialConfigDirectories() {
  std::vector<std::string> potential_config_dirs = {};
  const std::vector<std::string> dir_starts = {".", ".."};
  const std::vector<std::string> dir_middles = {"/.config", "/../.config"};
  const std::vector<std::string> dir_ends = {"", "/weather-forecast"};

  for (const std::string& dir_start : dir_starts) {
    for (const std::string& dir_middle : dir_middles) {
      for (const std::string& dir_end : dir_ends) {
        std::string potential_config_dir = dir_start;
        potential_config_dir += dir_middle;
        potential_config_dir += dir_end;
        potential_config_dirs.push_back(potential_config_dir);
      }
    }
  }

  if (IsWindows()) {
    std::string home_path_cmd = std::getenv("userprofile");
    std::string home_path_shell = std::getenv("HOME");
    potential_config_dirs.push_back(home_path_cmd + "/.config/weather-forecast");
    potential_config_dirs.push_back(home_path_shell + "/.config/weather-forecast");
  } else {
    potential_config_dirs.emplace_back("~/.config/weather-forecast");
    potential_config_dirs.emplace_back("/etc/weather-forecast");
  }

  return potential_config_dirs;
}
