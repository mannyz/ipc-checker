#include "run/types.hpp"

#include <stdexcept>

Mode Convert(std::string_view role, Mode) {
  if (role == "consumer") {
    return Mode::Consumer;
  } else if (role == "producer") {
    return Mode::Producer;
  } else if (role == "analyzer") {
    return Mode::Analyzer;
  } else if (role == "run_test") {
    return Mode::RunTest;
  } else {
    throw std::runtime_error("Unsupported mode type");
  }
}
