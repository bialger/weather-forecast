#ifndef ERROROUTPUT_HPP_
#define ERROROUTPUT_HPP_

#include <iostream>

/**\n Structure for error output in ArgumentParser */

struct ConditionalOutput {
  std::ostream& out_stream = std::cout;
  bool print_messages = false;
};

#endif //ERROROUTPUT_HPP_
