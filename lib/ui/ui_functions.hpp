#ifndef UI_FUNCTIONS_HPP_
#define UI_FUNCTIONS_HPP_

#include <iostream>
#include <vector>
#include <string>
#include <cstdint>

#include "lib/utils/utils.hpp"

int32_t StartConsoleUI(const std::vector<std::string>& args, std::ostream& out, std::istream& in);

std::vector<std::string> GetPotentialConfigDirectories();

#endif //UI_FUNCTIONS_HPP_
