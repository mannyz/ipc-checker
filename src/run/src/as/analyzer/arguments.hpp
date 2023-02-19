#pragma once

#include <argp.h>

#include <charconv>
#include <cstring>
#include <stdexcept>
#include <string>

#include "connection/connectors/types.hpp"
#include "run/as/common/arguments.hpp"

namespace {
namespace arguments {

constexpr auto usage = "analyzer <chunk's settings> <connection's settings>";

constexpr int kChunksTotalCount = 1000;
constexpr int kMeasurerProducer = 2000;
constexpr int kMeasurerConsumer = 3000;

argp_option options[] = {
    {0, 0, 0, 0, "Chunk's settings:"},
    {"chunks-total-count", kChunksTotalCount, "COUNT", 0,
     "Total COUNT of chunks that will be transferred"},
    {0, 0, 0, 0, "Measurer's settings:"},
    {"measurer-producer", kMeasurerProducer,
     "(netsock|pipe|shmem|uxsock)://<measurer_producer_connection_id>", 0,
     "Connection string for producer's measurements transfer. For example, "
     "\"shmem://measurer_producer_connection\" or "
     "\"netsock://localhost:<measurer_producer_connection_port>\""},
    {"measurer-consumer", kMeasurerConsumer,
     "(netsock|pipe|shmem|uxsock)://<measurer_consumer_connection_id>", 0,
     "Connection string for consumer's measurements transfer. For example, "
     "\"shmem://measurer_consumer_connection\" or "
     "\"netsock://localhost:<measurer_consumer_connection_port>\""},
    {0}};

class AnalizerArguments : public Arguments {
public:
  std::size_t chunks_total_count;
  ConnectionType measurer_producer_connection_type;
  std::string measurer_producer_connection_id;
  ConnectionType measurer_consumer_connection_type;
  std::string measurer_consumer_connection_id;

public:
  AnalizerArguments()
      : Arguments{{
            Arguments::Required("chunks-total-count", kChunksTotalCount,
                                [this](std::string_view value) {
                                  std::from_chars(std::begin(value),
                                                  std::end(value),
                                                  this->chunks_total_count);
                                }),
            Arguments::Required(
                "measurer-producer", kMeasurerProducer,
                [this](std::string_view value) {
                  auto type_n = value.find("://");
                  if (type_n == value.npos) {
                    throw std::runtime_error(
                        "Cannot parse connection type from argument");
                  }
                  this->measurer_producer_connection_type =
                      Convert(value.substr(0, type_n), ConnectionType{});
                  this->measurer_producer_connection_id =
                      value.substr(type_n + 3);
                }),
            Arguments::Required(
                "measurer-consumer", kMeasurerConsumer,
                [this](std::string_view value) {
                  auto type_n = value.find("://");
                  if (type_n == value.npos) {
                    throw std::runtime_error(
                        "Cannot parse connection type from argument");
                  }
                  this->measurer_consumer_connection_type =
                      Convert(value.substr(0, type_n), ConnectionType{});
                  this->measurer_consumer_connection_id =
                      value.substr(type_n + 3);
                }),
        }} {}
};

error_t parser(int key, char *arg, argp_state *state) {

  auto *arguments = static_cast<AnalizerArguments *>(state->input);

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
  case kMeasurerProducer:
  case kMeasurerConsumer:
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
