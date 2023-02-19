#pragma once

#include <charconv>
#include <cstring>

#include <argp.h>

#include "connection/connectors/types.hpp"
#include "run/as/common/arguments.hpp"

namespace {
namespace arguments {

constexpr auto usage = "run_test <chunk's settings> <connection's settings> "
                       "<environment's settings>";

constexpr int kChunksSize = 1000;
constexpr int kChunksTotalCount = 1001;
constexpr int kConnection = 2000;
constexpr int kConnectionBufferSize = 2001;
constexpr int kEnvironmentProducerCpu = 3000;
constexpr int kEnvironmentConsumerCpu = 3001;

argp_option options[] = {
    {0, 0, 0, 0, "Chunk's settings:"},
    {"chunks-size", kChunksSize, "BYTES", 0,
     "Use messages with BYTES size. Currently using only a fixed size for "
     "messages"},
    {"chunks-total-count", kChunksTotalCount, "COUNT", 0,
     "Total COUNT of chunks that will be transferred"},
    {0, 0, 0, 0, "Connection's settings:"},
    {"connection", kConnection, "(netsock|pipe|shmem|uxsock)", 0,
     "Use preferred connection type for transmission"},
    {"connection-buffer-size", kConnectionBufferSize, "BYTES", 0,
     "Use connection buffer with BYTES size"},
    {0, 0, 0, 0, "Environment's settings:"},
    {"environment-producer-cpu", kEnvironmentProducerCpu, "CPU_ID", 0,
     "Use cpu with CPU_ID number for producer's process"},
    {"environment-consumer-cpu", kEnvironmentConsumerCpu, "CPU_ID", 0,
     "Use cpu with CPU_ID number for consumer's process"},
    {0}};

class RunTestArguments : public Arguments {
public:
  std::size_t chunks_size;
  std::size_t chunks_total_count;
  ConnectionType connection_type;
  std::size_t connection_buffer_size;
  int environment_producer_cpu = -1;
  int environment_consumer_cpu = -1;

public:
  RunTestArguments()
      : Arguments({
            Arguments::Required("chunks-size", kChunksSize,
                                [this](std::string_view value) {
                                  std::from_chars(std::begin(value),
                                                  std::end(value),
                                                  this->chunks_size);
                                }),
            Arguments::Required("chunks-total-count", kChunksTotalCount,
                                [this](std::string_view value) {
                                  std::from_chars(std::begin(value),
                                                  std::end(value),
                                                  this->chunks_total_count);
                                }),
            Arguments::Required("connection", kConnection,
                                [this](std::string_view value) {
                                  this->connection_type =
                                      Convert(value, ConnectionType{});
                                }),
            Arguments::Required("connection-buffer-size", kConnectionBufferSize,
                                [this](std::string_view value) {
                                  std::from_chars(std::begin(value),
                                                  std::end(value),
                                                  this->connection_buffer_size);
                                }),
            Arguments::Optional(
                "environment-producer-cpu", kEnvironmentProducerCpu,
                [this](std::string_view value) {
                  std::from_chars(std::begin(value), std::end(value),
                                  this->environment_producer_cpu);
                }),
            Arguments::Optional(
                "environment-consumer-cpu", kEnvironmentConsumerCpu,
                [this](std::string_view value) {
                  std::from_chars(std::begin(value), std::end(value),
                                  this->environment_consumer_cpu);
                }),
        }) {}
};

error_t parser(int key, char *arg, argp_state *state) {
  auto *arguments = static_cast<RunTestArguments *>(state->input);

  switch (key) {
  case ARGP_KEY_INIT:
    state->next = 2;
    break;
  case ARGP_KEY_END:
    if (arguments->HasNotParsedRequiredArguments()) {
      argp_failure(state, 1, 0, "Too few required arguments");
    }
    break;
  case kChunksSize:
  case kChunksTotalCount:
  case kConnection:
  case kConnectionBufferSize:
  case kEnvironmentProducerCpu:
  case kEnvironmentConsumerCpu:
    arguments->ParseArgument(key, {arg, strlen(arg)});
    break;
  default:
    return ARGP_ERR_UNKNOWN;
  }
  return 0;
}

argp definition = {options, parser, usage, 0};

} // namespace arguments
} // namespace
