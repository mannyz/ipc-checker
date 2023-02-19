#include "run/as/run_test.hpp"

#include "arguments.hpp"

#include "connection/connectors/types.hpp"

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

#include <sys/wait.h>
#include <unistd.h>

namespace {

std::vector<char *> PackModeArguments(char *program,
                                      std::vector<std::string> &arguments) {
  std::vector<char *> execv_arguments;
  execv_arguments.reserve(arguments.size() + 2);
  execv_arguments.emplace_back(program);
  for (auto &arg : arguments) {
    execv_arguments.emplace_back(arg.data());
  }
  execv_arguments.emplace_back(nullptr);
  return execv_arguments;
}

enum class ConnectionSide {
  kConsumerProducerServer,
  kConsumerProducerClient,
  kMeasurerProducerServer,
  kMeasurerProducerClient,
  kMeasurerConsumerServer,
  kMeasurerConsumerClient,
};

std::string MakeConnectionString(ConnectionSide side, ConnectionType type) {
  switch (side) {
  case ConnectionSide::kConsumerProducerServer: {
    switch (type) {
    case ConnectionType::kNetSocket:
      return ToString(type) + "://:8000";
    case ConnectionType::kPipe:
    case ConnectionType::kSharedMemory:
    case ConnectionType::kUnixSocket:
      return ToString(type) + "://ipc_checker_connection_id";
    }
  } break;
  case ConnectionSide::kConsumerProducerClient: {
    switch (type) {
    case ConnectionType::kNetSocket:
      return ToString(type) + "://localhost:8000";
    case ConnectionType::kPipe:
    case ConnectionType::kSharedMemory:
    case ConnectionType::kUnixSocket:
      return ToString(type) + "://ipc_checker_connection_id";
    }
  } break;
  case ConnectionSide::kMeasurerProducerServer: {
    switch (type) {
    case ConnectionType::kNetSocket:
      return ToString(type) + "://:8100";
    case ConnectionType::kPipe:
    case ConnectionType::kSharedMemory:
    case ConnectionType::kUnixSocket:
      return ToString(type) + "://ipc_checker_measurer_producer_id";
    }
  } break;
  case ConnectionSide::kMeasurerProducerClient: {
    switch (type) {
    case ConnectionType::kNetSocket:
      return ToString(type) + "://localhost:8100";
    case ConnectionType::kPipe:
    case ConnectionType::kSharedMemory:
    case ConnectionType::kUnixSocket:
      return ToString(type) + "://ipc_checker_measurer_producer_id";
    }
  } break;
  case ConnectionSide::kMeasurerConsumerServer: {
    switch (type) {
    case ConnectionType::kNetSocket:
      return ToString(type) + "://:8200";
    case ConnectionType::kPipe:
    case ConnectionType::kSharedMemory:
    case ConnectionType::kUnixSocket:
      return ToString(type) + "://ipc_checker_measurer_consumer_id";
    }
  } break;
  case ConnectionSide::kMeasurerConsumerClient: {
    switch (type) {
    case ConnectionType::kNetSocket:
      return ToString(type) + "://localhost:8200";
    case ConnectionType::kPipe:
    case ConnectionType::kSharedMemory:
    case ConnectionType::kUnixSocket:
      return ToString(type) + "://ipc_checker_measurer_consumer_id";
    }
  } break;
  }

  throw std::runtime_error(
      (
          std::stringstream()
          << "Unexpected connection direction or type: direction="
          << static_cast<int>(side) << ", type=" << static_cast<int>(type))
          .str());
}

} // namespace

int runAsRunTest(int argc, char *argv[]) try {
  arguments::RunTestArguments arguments;
  argp_parse(&arguments::definition, argc, argv, 0, 0, &arguments);

  std::vector<std::vector<std::string>> modes_arguments = {
      {
          "analyzer",
          "--chunks-total-count=" +
              std::to_string(arguments.chunks_total_count),
          "--measurer-producer=" +
              MakeConnectionString(ConnectionSide::kMeasurerProducerServer,
                                   arguments.connection_type),
          "--measurer-consumer=" +
              MakeConnectionString(ConnectionSide::kMeasurerConsumerServer,
                                   arguments.connection_type),
      },
      {
          "consumer",
          "--chunks-total-count=" +
              std::to_string(arguments.chunks_total_count),
          "--connection=" +
              MakeConnectionString(ConnectionSide::kConsumerProducerServer,
                                   arguments.connection_type),
          "--connection-buffer-size=" +
              std::to_string(arguments.connection_buffer_size),
          "--measurer=" +
              MakeConnectionString(ConnectionSide::kMeasurerConsumerClient,
                                   arguments.connection_type),
          "--cpu=" + std::to_string(arguments.environment_consumer_cpu),
      },
      {
          "producer",
          "--chunks-size=" + std::to_string(arguments.chunks_size),
          "--chunks-total-count=" +
              std::to_string(arguments.chunks_total_count),
          "--connection=" +
              MakeConnectionString(ConnectionSide::kConsumerProducerClient,
                                   arguments.connection_type),
          "--connection-buffer-size=" +
              std::to_string(arguments.connection_buffer_size),
          "--measurer=" +
              MakeConnectionString(ConnectionSide::kMeasurerProducerClient,
                                   arguments.connection_type),
          "--cpu=" + std::to_string(arguments.environment_producer_cpu),
      },
  };

  for (auto &ma : modes_arguments) {
    auto child = fork();
    if (child == -1) {
      throw std::runtime_error("fork() failed");
    } else if (child) {
      std::cout << "[run_test] " << ma.at(0) << " was started..." << std::endl
                << std::flush;
      continue;
    }

    if (execv(argv[0], PackModeArguments(argv[0], ma).data()) == -1) {
      throw std::runtime_error("execl() failed");
    }
  }

  while (wait(nullptr) != -1)
    ;

  return 0;
} catch (const std::exception &e) {
  std::cout << "[run_test] An error occurred during work: " << e.what()
            << std::endl
            << std::flush;
  throw;
}
