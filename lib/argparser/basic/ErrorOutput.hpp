#ifndef ERROROUTPUT_HPP_
#define ERROROUTPUT_HPP_

#include <iostream>

/**\n Structure for error output in ArgumentParser */

struct ErrorOutput {
  std::ostream& error_stream = std::cout;
  bool print_errors = false;
};

#endif //ERROROUTPUT_HPP_
