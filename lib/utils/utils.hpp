#ifndef UTILS_HPP_
#define UTILS_HPP_

#include <cstdint>
#include <string_view>
#include <algorithm>
#include <vector>
#include <iostream>
#include <numeric>
#include <map>

#include "ConditionalOutput.hpp"

#if defined _WIN32 || defined _WIN64 || defined __CYGWIN__
#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef _WINDOWS_
#include <Windows.h>
#endif
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

void DisplayError(const std::string& message, ConditionalOutput error_output);

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

/**\n This function splits a string into a vector of strings. */

std::vector<std::string> Split(const std::string& str);

/**\n This function returns average value of the std::vector. */

template<typename T, typename U = double>
U CountAverage(const std::vector<T>& values) {
  static_assert(std::is_arithmetic<std::decay_t<decltype(*values.begin())>>::value,
                "Can only average arithmetic values");
  return static_cast<U>(std::accumulate(values.begin(), values.end(), static_cast<U>(0)) / values.size());
}

/**\n This function returns the most common value in std::vector of an arithmetic type */

template<class InputIt, class T = typename std::iterator_traits<InputIt>::value_type>
T MostCommon(InputIt begin, InputIt end) {
  std::map<T, int> counts;

  for (InputIt it = begin; it != end; ++it) {
    if (counts.find(*it) != counts.end()) {
      ++counts[*it];
    } else {
      counts[*it] = 1;
    }
  }

  return std::max_element(counts.begin(), counts.end(),
                          [](const std::pair<T, int>& pair1, const std::pair<T, int>& pair2) {
                            return pair1.second < pair2.second;
                          })->first;
}

/**\n This function returns the current time. */

std::string GetCurrentTime();

/**\n This function writes the current time to conditional stream. */

void WriteCurrentTime(ConditionalOutput& target);

#endif //UTILS_HPP_
