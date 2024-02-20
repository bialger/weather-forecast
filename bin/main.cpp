#include "lib/ui/TextUserInterface.hpp"

#include "lib/utils/utils.hpp"

int main(int argc, char** argv) {
  SetOutputToUnicode();
  TextUserInterface tui(std::cout, std::cerr, std::cin);
  return tui.Start(std::vector<std::string>(argv, argv + argc));
}
