#include "connection/connectors/types.hpp"

#include <sstream>
#include <stdexcept>

ConnectionType Convert(std::string_view connection, ConnectionType) {
  if (connection == "netsock") {
    return ConnectionType::kNetSocket;
  } else if (connection == "pipe") {
    return ConnectionType::kPipe;
  } else if (connection == "shmem") {
    return ConnectionType::kSharedMemory;
  } else if (connection == "uxsock") {
    return ConnectionType::kUnixSocket;
  }

  throw std::runtime_error((std::stringstream()
                            << "Unsupported connection type: name=\""
                            << connection << "\"")
                               .str());
}

std::string ToString(ConnectionType type) {
  switch (type) {
  case ConnectionType::kNetSocket:
    return "netsock";
  case ConnectionType::kPipe:
    return "pipe";
  case ConnectionType::kSharedMemory:
    return "shmem";
  case ConnectionType::kUnixSocket:
    return "uxsock";
  }

  throw std::runtime_error((std::stringstream()
                            << "Unimplemented connection type: {enum}=\""
                            << static_cast<int>(type) << "\"")
                               .str());
}
