#include "ui_functions.hpp"
#include "lib/argparser/ArgParser.hpp"

#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>
#include "ftxui/component/component.hpp"

int32_t StartConsoleUI(const std::vector<std::string>& args, std::ostream& out, std::istream& in) {
  ErrorOutput error_output = {out, true};
  ArgumentParser::ArgParser parser("weather-forecast");

  if (args.size() < 2) {
    out << "Insufficient arguments\n";
    return 1;
  }

  return 0;
}
