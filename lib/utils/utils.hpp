#ifndef UTILS_HPP_
#define UTILS_HPP_

#include <cstdint>
#include <string_view>
#include <iostream>

#if defined _WIN32 || defined _WIN64 || defined __CYGWIN__
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <Windows.h>
#else
#define STD_OUTPUT_HANDLE 0
#define HANDLE int
int GetStdHandle(int a);
int SetConsoleTextAttribute(int a, int b);
#endif

/**\n Structure for error output in ArgumentParser */

struct ErrorOutput {
  std::ostream& error_stream = std::cout;
  bool print_errors = false;
};

/**\n This function always returns true, because it is a default function */

bool AlwaysTrue(std::string& str);

/**\n This function sets the console text color to red. */

void SetRedColor();

/**\n This function resets the console text color to white. */

void ResetColor();

/**\n This function prints a error message. */

void DisplayError(const std::string& message, ErrorOutput error_output);

/**\n This function checks if the code is running on Windows. */

bool IsWindows();

/**\n The code validates the validity of a filename based on rules specific to
 * Windows. The filename should consist of alphanumeric characters, backslashes,
 * periods, hyphens, and spaces. Additionally, the code checks for consecutive
 * slashes in the filename, which is also considered invalid. */

bool IsValidFilename(std::string& pre_filename);

/**\n This function is a wrapper for the std::filesystem::is_regular_file function. */

bool IsRegularFile(std::string& filename);

/**\n This function is a wrapper for the std::filesystem::is_directory function. */

bool IsDirectory(std::string& dirname);

#endif //UTILS_HPP_
