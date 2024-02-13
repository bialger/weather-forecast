#include "lib/ui/ui_functions.hpp"

int main(int argc, char** argv) {
  return StartConsoleUI(std::vector<std::string>(argv, argv + argc), std::cout, std::cin);
}
