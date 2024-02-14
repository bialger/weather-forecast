#ifndef UI_FUNCTIONS_HPP_
#define UI_FUNCTIONS_HPP_

#include <iostream>
#include <vector>
#include <string>
#include <cstdint>

#include "lib/utils/utils.hpp"

#include <nlohmann/json.hpp>
#include <cpr/cpr.h>

int32_t StartConsoleUI(const std::vector<std::string>& args, std::ostream& out, std::istream& in);

#endif //UI_FUNCTIONS_HPP_
