#include <iostream>
#include <filesystem>

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

void DisplayError(const std::string& message, ErrorOutput error_output) {
  if (!error_output.print_errors) {
    return;
  }

  if (&error_output.error_stream == &std::cout) {
    SetRedColor();
  }

  error_output.error_stream << message;

  if (&error_output.error_stream == &std::cout) {
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
      char current = pre_filename[position];
      if (!(std::isalnum(current) || current == '\\' || current == '.' ||
          current == '-' || current == ' ' || current == '_')) {
        return false;
      }
    }
  }

  /* This Windows-specific check is important because different code pages can
   * corrupt non-alphanumeric filenames, but UNIX-like systems (like MacOS or
   * Linux) handle unicode correctly. */

  for (uint64_t position = 0; position < pre_filename.size() - 1; ++position) {
    char current = pre_filename[position];
    char next = pre_filename[position + 1];

    if ((current == '\\' || current == '/') && next == current) {
      return false;
    }
  }

  return true;
}

bool IsRegularFile(std::string& filename) {
  std::filesystem::path path(filename);
  return std::filesystem::is_regular_file(path);
}

bool IsDirectory(std::string& dirname) {
  std::filesystem::path path(dirname);
  return std::filesystem::is_directory(path);
}

/* The code provides dummy function definitions for Windows console-related
 * functions when the code is being compiled in a non-Windows environment.
 * This ensures that the code can compile and run without errors in such
 * environments. The dummy functions have minimal functionality and simply
 * return their input parameters. */

#if not(defined _WIN32 || defined _WIN64 || defined __CYGWIN__)
int SetConsoleOutputCP(int a) {return a;}
int SetConsoleCP(int a) {return a;}
int GetStdHandle(int a) {return a;}
int SetConsoleTextAttribute(int a, int b) {return a + b;}
#endif