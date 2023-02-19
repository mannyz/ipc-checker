#pragma once

#include <string>
#include <string_view>

enum class ConnectionType { kNetSocket, kPipe, kSharedMemory, kUnixSocket };

ConnectionType Convert(std::string_view connection, ConnectionType);
std::string ToString(ConnectionType);
