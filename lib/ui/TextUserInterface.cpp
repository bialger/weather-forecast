#include <fstream>
#include <iterator>
#include <functional>

#include "TextUserInterface.hpp"
#include "TuiWorker.hpp"
#include "ConfigParser.hpp"

const std::string TextUserInterface::kProgramName = "weather-forecast";
const std::string TextUserInterface::kVersion = "1.3.6 Ferdinando II de' Medici";
const CompositeString TextUserInterface::kDefaultConfigPath = "default_config.json";
const CompositeString TextUserInterface::kDefaultLogPath = "Print to standard output";

TextUserInterface::TextUserInterface(std::ostream& out, std::ostream& err, std::istream& in)
    : out_(out), in_(in), error_output_{err, true}, parser_(kProgramName + " " + kVersion) {
  config_path_ = kDefaultConfigPath;

  std::function<bool(std::string&)> IsGoodIntervalLength = [&](std::string& str_size) -> bool {
    int32_t block_size = std::stoi(str_size);
    return block_size > ConfigParser::kLowerLimitIntervalSize && block_size < ConfigParser::kUpperLimitIntervalSize;
  };

  std::function<bool(std::string&)> IsGoodDaysCount = [&](std::string& str_count) -> bool {
    int32_t days_count = std::stoi(str_count);
    return days_count > Forecaster::kLowerLimitDaysCount && days_count < Forecaster::kUpperLimitDaysCount;
  };

  std::function<bool(std::string&)> IsNotADirectory = [&](std::string& potential_filename) -> bool {
    return !IsDirectory(potential_filename);
  };

  std::string interval_description = "Initial time between weather forecast updates in hours. Should be more than "
      + std::to_string(ConfigParser::kLowerLimitIntervalSize) + " and less than "
      + std::to_string(ConfigParser::kUpperLimitIntervalSize)
      + ". The default value means using the parameter from the configuration file.";

  std::string days_count_description = "Initial number of days for which the forecast is shown. Should be more than "
      + std::to_string(Forecaster::kLowerLimitDaysCount) + " and less than "
      + std::to_string(Forecaster::kUpperLimitDaysCount)
      + ". The default value means using the parameter from the configuration file.";

  parser_.AddCompositeArgument('c', "config", "Path to the configuration file").Default(kDefaultConfigPath)
      .StoreValue(config_path_).AddValidate(&IsValidFilename).AddIsGood(&IsRegularFile);
  parser_.AddCompositeArgument('L', "log-file", "Path to the log file. If not specified, prints to standard output")
      .Default(kDefaultLogPath).AddValidate(&IsValidFilename).AddIsGood(IsNotADirectory);
  parser_.AddStringArgument('l', "location", "Name of the location for which the forecast is requested")
      .Default(ConfigParser::kDefaultLocation);
  parser_.AddIntArgument('i', "interval", interval_description)
      .Default(ConfigParser::kLowerLimitIntervalSize).AddIsGood(IsGoodIntervalLength);
  parser_.AddIntArgument('d', "days-count", days_count_description)
      .Default(Forecaster::kLowerLimitDaysCount).AddIsGood(IsGoodDaysCount);
  parser_.AddFlag('v', "verbose", "Show additional information.");
  parser_.AddHelp('h', "help", "A program for displaying the weather forecast in the terminal.");
}

int32_t TextUserInterface::Start(const std::vector<std::string>& args) {
  const std::vector<std::string> potential_config_dirs = GetPotentialConfigDirectories();

  if (!parser_.Parse(args, {error_output_.out_stream, error_output_.print_messages})) {
    out_ << parser_.HelpDescription();
    return 1;
  }

  if (parser_.Help()) {
    out_ << parser_.HelpDescription();
    return 0;
  }

  ConditionalOutput logs_output{background_logs_, parser_.GetFlag("verbose")};

  WriteCurrentTime(logs_output);
  logs_output << "PRE-TUI: Verbose mode is enabled.\n";
  WriteCurrentTime(logs_output);
  logs_output << "PRE-TUI: Parsed successfully with following arguments:\n";
  WriteCurrentTime(logs_output);
  logs_output << "PRE-TUI: ";

  for (const std::string& arg : args) {
    logs_output << arg << " ";
  }

  logs_output << "\n";
  WriteCurrentTime(logs_output);
  logs_output << "PRE-TUI: Parsing result:\n";
  WriteCurrentTime(logs_output);
  logs_output << "PRE-TUI: --location: " << parser_.GetStringValue("location") << "\n";
  WriteCurrentTime(logs_output);
  logs_output << "PRE-TUI: --config: " << config_path_ << "\n";
  WriteCurrentTime(logs_output);
  logs_output << "PRE-TUI: --log-file: " << parser_.GetCompositeValue("log-file") << "\n";
  WriteCurrentTime(logs_output);
  logs_output << "PRE-TUI: --interval: " << parser_.GetIntValue("interval") << "\n";
  WriteCurrentTime(logs_output);
  logs_output << "PRE-TUI: --days-count: " << parser_.GetIntValue("days-count") << "\n";
  WriteCurrentTime(logs_output);
  logs_output << "PRE-TUI: --verbose: " << parser_.GetFlag("verbose") << "\n";
  WriteCurrentTime(logs_output);
  logs_output << "PRE-TUI: --help: " << parser_.Help() << "\n";

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

  WriteCurrentTime(logs_output);
  logs_output << "PRE-TUI: Configuration file was found: " << config_path_ << "\n";
  WriteCurrentTime(logs_output);
  logs_output << "PRE-TUI: Parsing configuration file...\n";

  ConfigParser config(parser_.GetIntValue("interval"),
                      parser_.GetIntValue("days-count"),
                      parser_.GetStringValue("location"),
                      config_path_,
                      error_output_);

  int32_t result = config.ParseConfig();

  if (result != 0) {
    return result;
  }

  WriteCurrentTime(logs_output);
  logs_output << "PRE-TUI: Configuration file was parsed successfully.\n";
  WriteCurrentTime(logs_output);
  logs_output << "PRE-TUI: Configuration:\n";
  WriteCurrentTime(logs_output);
  logs_output << "PRE-TUI: Interval: " << config.GetInterval() << "\n";
  WriteCurrentTime(logs_output);
  logs_output << "PRE-TUI: Days count: " << config.GetDaysCount() << "\n";
  WriteCurrentTime(logs_output);
  logs_output << "PRE-TUI: Location index: " << config.GetLocationIndex() << "\n";
  WriteCurrentTime(logs_output);
  logs_output << "PRE-TUI: Locations: " << "\n";

  for (const auto& location : config.GetLocations()) {
    WriteCurrentTime(logs_output);
    logs_output << "PRE-TUI: " << location << "\n";
  }

  result = BeginForecast(config);

  if (parser_.GetCompositeValue("log-file") == kDefaultLogPath) {
    out_ << background_logs_.str();
  } else {
    std::ofstream log_file(parser_.GetCompositeValue("log-file"));

    if (!log_file.is_open()) {
      DisplayError("Failed to open log file!\n", error_output_);
      out_ << background_logs_.str();
      DisplayError(background_errors_.str(), error_output_);
      DisplayError("Program terminated with error code " + std::to_string(result) + ".\n", error_output_);
      return 1;
    }

    log_file << background_logs_.str();
    log_file.close();
  }

  if (result != 0) {
    DisplayError(background_errors_.str(), error_output_);
    DisplayError("Program terminated with error code " + std::to_string(result) + ".\n", error_output_);
  }

  return result;
}

int32_t TextUserInterface::BeginForecast(const ConfigParser& config) {
  ConditionalOutput background_errors{background_errors_, true};
  ConditionalOutput background_logs{background_logs_, parser_.GetFlag("verbose")};

  WriteCurrentTime(background_logs);
  background_logs << "PRE-TUI: Forecasting...\n";

  Forecaster forecaster(
      config.GetDaysCount(),
      config.GetLocationIndex(),
      config.GetLocations(),
      config.GetApiKey(),
      config.GetConfigDir(),
      background_errors,
      background_logs
  );

  int32_t result = forecaster.ObtainForecast();

  WriteCurrentTime(background_logs);
  background_logs << "PRE-TUI: First forecast obtained.\n";

  if (result != 0) {
    return result;
  }

  WriteCurrentTime(background_logs);
  background_logs << "PRE-TUI: Starting TUI in thread " << std::this_thread::get_id() << "\n";

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
    try {
      std::string home_path_cmd = std::getenv("userprofile");
      potential_config_dirs.push_back(home_path_cmd + "/.config/weather-forecast");
    } catch (const std::exception&) {}

    try {
      std::string home_path_shell = std::getenv("HOME");
      potential_config_dirs.push_back(home_path_shell + "/.config/weather-forecast");
    } catch (const std::exception&) {}
  } else {
    potential_config_dirs.emplace_back("~/.config/weather-forecast");
    potential_config_dirs.emplace_back("/etc/weather-forecast");
  }

  return potential_config_dirs;
}
