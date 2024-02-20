#ifndef UTILS_HPP_
#define UTILS_HPP_

#include <cstdint>
#include <string_view>
#include <algorithm>
#include <vector>
#include <iostream>
#include <numeric>
#include <map>

#include "ErrorOutput.hpp"

#if defined _WIN32 || defined _WIN64 || defined __CYGWIN__
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <Windows.h>
#else
#define CP_UTF8 0
#define STD_OUTPUT_HANDLE 0
#define HANDLE int
inline int SetConsoleOutputCP(int a);
inline int SetConsoleCP(int a);
inline int GetStdHandle(int a);
inline int SetConsoleTextAttribute(int a, int b);
#endif

/**\n This function sets the console text color to red. */

void SetRedColor();

/**\n This function resets the console text color to white. */

void ResetColor();

/**\n This function prints a error message. */

void DisplayError(const std::string& message, ErrorOutput error_output);

/**\n This function sets the output to UTF-8 on Windows. */

void SetOutputToUnicode();

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

/**\n This function returns the size of a file. Does not use tellg(). */

std::streamsize GetFileSize(const std::string& filename);

/**\n This function returns the contents of a file as a string. */

std::string GetStringFromFile(const std::string& filename);

/**\n This function writes string to stream. */

void WriteStringToStream(const std::string& content, std::ostream& target);

/**\n This function returns average value of the std::vector. */

template<typename T, typename U = double>
U CountAverage(const std::vector<T>& values) {
  static_assert(std::is_arithmetic<std::decay_t<decltype(*values.begin())>>::value,
                "Can only average arithmetic values");
  return static_cast<U>(std::accumulate(values.begin(), values.end(), static_cast<U>(0)) / values.size());
}

/**\n This function returns the most common value in std::vector of an arithmetic type */

template<typename T>
T GetMostCommon(const std::vector<T>& values) {
  static_assert(std::is_arithmetic<std::decay_t<decltype(*values.begin())>>::value,
                "Can only find the most common arithmetic value");
  std::map<T, T> map;
  for (auto i : values)
    map[i]++;

  return std::max_element(map.begin(),
                          map.end(),
                          [](const std::pair<int, int>& a, const std::pair<int, int>& b) {
                            return a.second < b.second;
                          })->first;
}

#endif //UTILS_HPP_
