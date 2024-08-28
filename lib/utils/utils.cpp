#include <iostream>
#include <filesystem>
#include <fstream>
#include <vector>
#include <limits>
#include <iterator>
#include <sstream>
#include <iomanip>

#include "utils.hpp"

void SetRedColor() {
  /* Changes the color of the console output to red if not running
   * on Windows. */

  std::cout << (IsWindows() ? "" : "\x1B[31m");

  /* Conditional block to set the console text color to red if the program is
   * running on Windows, using the Windows API functions. */

  if (IsWindows()) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, 12);
  }
}

void ResetColor() {
  if (IsWindows()) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, 15);
  }

  std::cout << (IsWindows() ? "" : "\x1B[0m");
}

void DisplayError(const std::string& message, ConditionalOutput error_output) {
  if (!error_output.print_messages) {
    return;
  }

  const bool is_console_output = &error_output.out_stream == &std::cout || &error_output.out_stream == &std::cerr;

  if (is_console_output) {
    SetRedColor();
  }

  error_output.out_stream << message;

  if (is_console_output) {
    ResetColor();
  }
}

void SetOutputToUnicode() {
  if (IsWindows()) {
    setlocale(LC_ALL, ".UTF8");
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
  }
}

bool IsWindows() {
  return
#if defined _WIN32 || defined _WIN64 || defined __CYGWIN__
    true
#else
      false
#endif
      ;
}

bool IsValidFilename(std::string& pre_filename) {
  if (pre_filename.size() > 7) {
    if (pre_filename.substr(0, 7) == "file://") {
      pre_filename = pre_filename.substr(7);
    }
  }

  if (IsWindows() && pre_filename.size() > 2) {
    for (uint64_t position = 2; position < pre_filename.size(); ++position) {
      const char current = pre_filename[position];
      if (!(std::isalnum(current) || current == '\\' || current == '.' ||
        current == '-' || current == ' ' || current == '_')) {
        return false;
      }
    }
  }

  /* This Windows-specific check is important because different code pages can
   * corrupt non-alphanumeric filenames, but UNIX-like systems (like MacOS or
   * Linux) handle Unicode correctly. */

  for (uint64_t position = 0; position < pre_filename.size() - 1; ++position) {
    const char current = pre_filename[position];
    const char next = pre_filename[position + 1];

    if ((current == '\\' || current == '/') && next == current) {
      return false;
    }
  }

  return true;
}

bool IsRegularFile(std::string& filename) {
  const std::filesystem::path path(filename);
  return std::filesystem::is_regular_file(path);
}

bool IsDirectory(std::string& dirname) {
  const std::filesystem::path path(dirname);
  return std::filesystem::is_directory(path);
}

std::streamsize GetFileSize(const std::string& filename) {
  std::ifstream file(filename, std::ios::in | std::ios::binary);
  file.ignore(std::numeric_limits<std::streamsize>::max());
  const std::streamsize length = file.gcount();
  file.clear(); //  Since ignore will have set eof.
  file.seekg(0, std::ios_base::beg);
  return length;
}

std::string GetStringFromFile(const std::string& filename) {
  std::ifstream ifs(filename.c_str(), std::ios::in | std::ios::binary);

  const std::streamsize file_size = GetFileSize(filename);

  std::vector<char> bytes(file_size);
  ifs.read(bytes.data(), file_size);

  return {bytes.data(), static_cast<size_t>(file_size)};
}

void WriteStringToStream(const std::string& content, std::ostream& target) {
  target.write(content.c_str(), static_cast<std::streamsize>(content.size()));
}

std::vector<std::string> Split(const std::string& str) {
  std::istringstream iss(str);

  return {std::istream_iterator<std::string>(iss), std::istream_iterator<std::string>()};
}

std::string GetStringCurrentTime() {
  std::ostringstream oss;
  ConditionalOutput target{oss, true};
  WriteCurrentTime(target);
  return oss.str();
}

void WriteCurrentTime(ConditionalOutput& target) {
  if (!target.print_messages) {
    return;
  }

  const auto now = std::chrono::system_clock::now();
  const std::time_t now_c = std::chrono::system_clock::to_time_t(now);
  const std::tm* parts = std::localtime(&now_c);

  const std::ios_base::fmtflags old_flags = target.out_stream.flags();
  const std::streamsize old_precision = target.out_stream.precision();

  target << std::put_time(parts, "%Y-%m-%d %H:%M:%S.") << std::setfill('0') << std::setw(3)
      << (std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count() % 1000) << " ";

  target.out_stream.flags(old_flags);
  target.out_stream.precision(old_precision);
}
