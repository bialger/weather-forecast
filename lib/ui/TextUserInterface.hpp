#ifndef TEXTUSERINTERFACE_HPP_
#define TEXTUSERINTERFACE_HPP_

#include <iostream>
#include <vector>
#include <string>
#include <cstdint>

#include "lib/utils/utils.hpp"
#include "lib/argparser/ArgParser.hpp"
#include "lib/forecast/Forecaster.hpp"

class TextUserInterface {
 public:
  static const std::string kProgramName;
  static const CompositeString kDefaultConfigPath;

  TextUserInterface(std::ostream& out, std::ostream& err, std::istream& in);
  TextUserInterface() = delete;

  int32_t Start(const std::vector<std::string>& args);

  static std::vector<std::string> GetPotentialConfigDirectories();

 private:
  static const std::string kIntervalDescription;
  static const std::string kDaysCountDescription;

  std::ostream& out_;
  std::istream& in_;
  ErrorOutput error_output_;
  std::ostringstream background_err_{};
  ArgumentParser::ArgParser parser_;
  CompositeString config_path_;

  int32_t BeginForecast();
};

#endif //TEXTUSERINTERFACE_HPP_
