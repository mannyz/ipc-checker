#pragma once

#include <charconv>
#include <cstring>
#include <optional>
#include <stdexcept>

#include <argp.h>

#include "connection/connectors/types.hpp"
#include "run/as/common/arguments.hpp"

namespace {
namespace arguments {

constexpr auto usage = "consumer <chunk's settings> <connection's settings> "
                       "<environment's settings>";

constexpr int kChunksTotalCount = 1000;
constexpr int kConnection = 2000;
constexpr int kConnectionBufferSize = 2001;
constexpr int kMeasurer = 3000;
constexpr int kCpu = 4000;

argp_option options[] = {
    {0, 0, 0, 0, "Chunk's settings:"},
    {"chunks-total-count", kChunksTotalCount, "COUNT", 0,
     "Total COUNT of chunks that will be transferred"},
    {0, 0, 0, 0, "Connections's settings:"},
    {"connection", kConnection, "(netsock|pipe|shmem|uxsock)://<connection_id>",
     0,
     "Connection string for data transfer. For example, "
     "\"shmem://producer_consumer_connection\" or "
     "\"netsock://localhost:<producer_consumer_connection_port>\""},
    {"connection-buffer-size", kConnectionBufferSize, "BYTES", 0,
     "Use connection buffer with BYTES size"},
    {0, 0, 0, 0, "Measurer's settings:"},
    {"measurer", kMeasurer,
     "(netsock|pipe|shmem|uxsock)://<measurer_consumer_connection_id>", 0,
     "Connection string for measurements transfer. For example, "
     "\"shmem://measurer_consumer_connection\" or "
     "\"netsock://localhost:<measurer_consumer_connection_port>\""},
    {0, 0, 0, 0, "Environment's settings:"},
    {"cpu", kCpu, "CPU_ID", 0, "Use cpu with CPU_ID number for process"},
    {0}};

class ConsumerArguments : public Arguments {
public:
  std::size_t chunks_total_count;
  ConnectionType connection_type;
  std::string connection_id;
  std::optional<std::size_t> connection_buffer_size;
  ConnectionType measurer_connection_type;
  std::string measurer_connection_id;
  int cpu = -1;

public:
  ConsumerArguments()
      : Arguments{{
            Arguments::Required("chunks-total-count", kChunksTotalCount,
                                [this](std::string_view value) {
                                  std::from_chars(std::begin(value),
                                                  std::end(value),
                                                  this->chunks_total_count);
                                }),
            Arguments::Required(
                "connection", kConnection,
                [this](std::string_view value) {
                  auto type_n = value.find("://");
                  if (type_n == value.npos) {
                    throw std::runtime_error(
                        "Cannot parse connection type from argument");
                  }
                  this->connection_type =
                      Convert(value.substr(0, type_n), ConnectionType{});
                  this->connection_id = value.substr(type_n + 3);
                }),
            Arguments::Optional("connection-buffer-size", kConnectionBufferSize,
                                [this](std::string_view value) {
                                  std::size_t connection_buffer_size;
                                  std::from_chars(std::begin(value),
                                                  std::end(value),
                                                  connection_buffer_size);
                                  this->connection_buffer_size =
                                      connection_buffer_size;
                                }),
            Arguments::Required(
                "measurer", kMeasurer,
                [this](std::string_view value) {
                  auto type_n = value.find("://");
                  if (type_n == value.npos) {
                    throw std::runtime_error(
                        "Cannot parse connection type from argument");
                  }
                  this->measurer_connection_type =
                      Convert(value.substr(0, type_n), ConnectionType{});
                  this->measurer_connection_id = value.substr(type_n + 3);
                }),
            Arguments::Optional("environment-cpu", kCpu,
                                [this](std::string_view value) {
                                  std::from_chars(std::begin(value),
                                                  std::end(value), this->cpu);
                                }),
        }} {}
};

error_t parser(int key, char *arg, argp_state *state) {

  auto *arguments = static_cast<ConsumerArguments *>(state->input);

  switch (key) {
  case ARGP_KEY_INIT:
    state->next = 2;
    break;
  case ARGP_KEY_END:
    if (arguments->HasNotParsedRequiredArguments()) {
      argp_failure(state, 1, 0, "Too few required arguments");
    }
    break;
  case kChunksTotalCount:
  case kConnection:
  case kConnectionBufferSize:
  case kMeasurer:
  case kCpu:
    arguments->ParseArgument(key, {arg, strlen(arg)});
    break;
  default:
    return ARGP_ERR_UNKNOWN;
  }
  return 0;
}

argp definitions = {options, parser, usage, 0};

} // namespace arguments
} // namespace
