#include <fstream>

#include "ui_functions.hpp"
#include "lib/argparser/ArgParser.hpp"
#include "lib/ftxui/ftxui_interface.hpp"

#include <nlohmann/json.hpp>
#include <cpr/cpr.h>

int32_t StartConsoleUI(const std::vector<std::string>& args, std::ostream& out, std::istream& in) {
  SetOutputToUnicode();
  constexpr int32_t kMinimalIntervalSize = 0; // that means use one from config
  constexpr int32_t kMaximalIntervalSize = 48;
  const std::vector<std::string> potential_config_dirs =
      {"./", "./.config", "../.config", "../../.config", "~/config/weather-forecast/", "/etc/weather-forecast/"};
  ErrorOutput error_output = {out, true};
  ArgumentParser::ArgParser parser("weather-forecast");
  CompositeString default_config_path = "default_config.json";
  CompositeString config_path;
  std::string default_location = "First location in config";
  std::string interval_description = "Time between weather forecast updates in hours. Should not be more than "
      + std::to_string(kMaximalIntervalSize) + " and less than "
      + std::to_string(kMinimalIntervalSize)
      + ". The default value means using the parameter from the configuration file.";

  auto IsGoodIntervalLength = [&](std::string& str_size) -> bool {
    int32_t block_size = std::stoi(str_size);
    return block_size > kMinimalIntervalSize && block_size <= kMaximalIntervalSize;
  };

  parser.AddCompositeArgument('c', "config", "Path to the configuration file").Default(default_config_path)
      .StoreValue(config_path).AddValidate(&IsValidFilename).AddIsGood(&IsRegularFile);
  parser.AddStringArgument('l', "location", "Name of the location for which the forecast is requested").Default(
      default_location);
  parser.AddIntArgument('i',
                        "interval",
                        interval_description.c_str())
      .Default(kMinimalIntervalSize).AddIsGood(IsGoodIntervalLength);
  parser.AddHelp('h', "help", "Program to show weather forecast");

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
  }

  std::ifstream file(config_path);
  nlohmann::json config = nlohmann::json::parse(file);

  return 0;
}
