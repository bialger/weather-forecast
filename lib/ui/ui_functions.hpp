#ifndef UI_FUNCTIONS_HPP_
#define UI_FUNCTIONS_HPP_

#include <iostream>
#include <vector>
#include <string>
#include <cstdint>

int32_t StartConsoleUI(const std::vector<std::string>& args, std::ostream& out, std::istream& in);

int32_t BeginForecast(int32_t interval,
                      int32_t days_count,
                      const std::string& location,
                      const std::string& config_path,
                      std::ostream& out,
                      std::istream& in);

std::vector<std::string> GetPotentialConfigDirectories();

#endif //UI_FUNCTIONS_HPP_
