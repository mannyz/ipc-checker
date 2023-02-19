#pragma once

#include <string>

namespace utils {

struct HostAndPort {
  std::string host;
  int port;
};

HostAndPort ParseHostAndPort(const std::string &connection_string);

} // namespace utils
