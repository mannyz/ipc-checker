#include "run/as/common/utils.hpp"

#include <stdexcept>

namespace utils {

HostAndPort ParseHostAndPort(const std::string &connection_string) {
  auto colon_symbol_pos = connection_string.find(":");
  if (colon_symbol_pos == connection_string.npos) {
    throw std::runtime_error("Cannot find colon symbol in connection string");
  }

  return {.host = connection_string.substr(0, colon_symbol_pos),
          .port = std::stoi(connection_string.substr(colon_symbol_pos + 1))};
}

} // namespace utils
