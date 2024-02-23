#include <fstream>
#include <iterator>
#include <functional>

#include "TextUserInterface.hpp"
#include "TuiWorker.hpp"
#include "ConfigParser.hpp"

const std::string TextUserInterface::kProgramName = "weather-forecast";
const CompositeString TextUserInterface::kDefaultConfigPath = "default_config.json";
const std::string TextUserInterface::kIntervalDescription =
    "Initial time between weather forecast updates in hours. Should be more than "
        + std::to_string(ConfigParser::kLowerLimitIntervalSize) + " and less than "
        + std::to_string(ConfigParser::kUpperLimitIntervalSize)
        + ". The default value means using the parameter from the configuration file.";
const std::string TextUserInterface::kDaysCountDescription =
    "Initial number of days for which the forecast is shown. Should be more than "
        + std::to_string(Forecaster::kLowerLimitDaysCount) + " and less than "
        + std::to_string(Forecaster::kUpperLimitDaysCount)
        + ". The default value means using the parameter from the configuration file.";

TextUserInterface::TextUserInterface(std::ostream& out, std::ostream& err, std::istream& in)
    : out_(out), in_(in), error_output_{err, true}, parser_(kProgramName.c_str()) {
  config_path_ = kDefaultConfigPath;

  std::function<bool(std::string&)> IsGoodIntervalLength = [&](std::string& str_size) -> bool {
    int32_t block_size = std::stoi(str_size);
    return block_size > ConfigParser::kLowerLimitIntervalSize && block_size < ConfigParser::kUpperLimitIntervalSize;
  };

  std::function<bool(std::string&)> IsGoodDaysCount = [&](std::string& str_count) -> bool {
    int32_t days_count = std::stoi(str_count);
    return days_count > Forecaster::kLowerLimitDaysCount && days_count < Forecaster::kUpperLimitDaysCount;
  };

  parser_.AddCompositeArgument('c', "config", "Path to the configuration file").Default(kDefaultConfigPath)
      .StoreValue(config_path_).AddValidate(&IsValidFilename).AddIsGood(&IsRegularFile);
  parser_.AddStringArgument('l', "location", "Name of the location for which the forecast is requested")
      .Default(ConfigParser::kDefaultLocation);
  parser_.AddIntArgument('i',
                         "interval",
                         kIntervalDescription.c_str())
      .Default(ConfigParser::kLowerLimitIntervalSize).AddIsGood(IsGoodIntervalLength);
  parser_.AddIntArgument('d',
                         "days-count",
                         kDaysCountDescription.c_str())
      .Default(Forecaster::kLowerLimitDaysCount).AddIsGood(IsGoodDaysCount);
  parser_.AddHelp('h', "help", "A program for displaying the weather forecast in the terminal.");
}

int32_t TextUserInterface::Start(const std::vector<std::string>& args) {
  const std::vector<std::string> potential_config_dirs = GetPotentialConfigDirectories();

  if (!parser_.Parse(args, error_output_)) {
    out_ << parser_.HelpDescription();
    return 1;
  }

  if (parser_.Help()) {
    out_ << parser_.HelpDescription();
    return 0;
  }

  if (config_path_ == kDefaultConfigPath) {
    for (const std::string& potential_config_dir : potential_config_dirs) {
      if (std::filesystem::is_regular_file(potential_config_dir + "/" + config_path_)) {
        config_path_ = potential_config_dir + "/" + config_path_;
        break;
      }
    }
  }

  if (!IsRegularFile(config_path_)) {
    DisplayError("No configuration file was found!\n", error_output_);
    out_ << parser_.HelpDescription();
    return 1;
  }

  ConfigParser config(parser_.GetIntValue("interval"),
                      parser_.GetIntValue("days-count"),
                      parser_.GetStringValue("location"),
                      config_path_,
                      error_output_);

  int32_t result = config.ParseConfig();

  if (result != 0) {
    return result;
  }

  result = BeginForecast(config);

  if (result != 0) {
    DisplayError(background_output_.str(), error_output_);
  } else {
    out_ << background_output_.str();
  }

  return result;
}

int32_t TextUserInterface::BeginForecast(const ConfigParser& config) {
  ErrorOutput background_output{background_output_, true};

  Forecaster forecaster(
      config.GetDaysCount(),
      config.GetLocationIndex(),
      config.GetLocations(),
      config.GetApiKey(),
      config.GetConfigDir(),
      background_output
  );

  int32_t result = forecaster.ObtainForecast();

  if (result != 0) {
    return result;
  }

  TuiWorker worker(forecaster, config.GetInterval());
  result = worker.Run();

  return result;
}

std::vector<std::string> TextUserInterface::GetPotentialConfigDirectories() {
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
