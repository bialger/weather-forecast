#ifndef TEXTUSERINTERFACE_HPP_
#define TEXTUSERINTERFACE_HPP_

#include <vector>
#include <string>
#include <cstdint>

#include <argparser/ArgParser.hpp>

#include "ConfigParser.hpp"

#include "lib/utils/utils.hpp"
#include "lib/forecast/Forecaster.hpp"

class TextUserInterface {
 public:
  static const std::string kProgramName;
  static const std::string kVersion;
  static const CompositeString kDefaultConfigPath;
  static const CompositeString kDefaultLogPath;

  TextUserInterface(std::ostream& out, std::ostream& err, std::istream& in);
  TextUserInterface() = delete;

  int32_t Start(const std::vector<std::string>& args);

  static std::vector<std::string> GetPotentialConfigDirectories();

 private:
  std::ostream& out_;
  std::istream& in_;
  ConditionalOutput error_output_;
  std::ostringstream background_errors_{};
  std::ostringstream background_logs_{};
  ArgumentParser::ArgParser parser_;
  CompositeString config_path_;

  int32_t BeginForecast(const ConfigParser& config);
};

#endif //TEXTUSERINTERFACE_HPP_
