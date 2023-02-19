#include "actors/analyzer.hpp"
#include "arguments.hpp"
#include "run/as/analyzer.hpp"
#include "run/as/common/maker.hpp"
#include "run/as/common/utils.hpp"

#include "connection/buffer/buffer.hpp"
#include "connection/connectors/net_socket/server.hpp"
#include "connection/connectors/net_socket/server_impl.hpp"
#include "connection/connectors/pipe/server.hpp"
#include "connection/connectors/pipe/server_impl.hpp"
#include "connection/connectors/shared_memory/server.hpp"
#include "connection/connectors/shared_memory/server_impl.hpp"
#include "connection/connectors/unix_socket/server.hpp"
#include "connection/connectors/unix_socket/server_impl.hpp"
#include "connection/packers/local_packer/local_packer.hpp"
#include "connection/packers/network_packer/network_packer.hpp"
#include "connection/packers/objects/time_point.hpp"
#include "measurer/measurer.hpp"
#include "statistics/statistics.hpp"
#include "statistics/statistics_impl.hpp"

namespace {

struct StatisticsParams {
  StatisticsParams(const arguments::AnalizerArguments &arguments) {}
};

struct MeasurerProducerParams {
  std::size_t chunks_total_count;

  MeasurerProducerParams(const arguments::AnalizerArguments &arguments)
      : chunks_total_count(arguments.chunks_total_count) {}
};

struct MeasurerConsumerParams {
  std::size_t chunks_total_count;

  MeasurerConsumerParams(const arguments::AnalizerArguments &arguments)
      : chunks_total_count(arguments.chunks_total_count) {}
};

struct MeasurerProducerConnectionPackerParams {
  ConnectionType type;

  MeasurerProducerConnectionPackerParams(
      const arguments::AnalizerArguments &arguments)
      : type(arguments.measurer_producer_connection_type) {}
};

struct MeasurerConsumerConnectionPackerParams {
  ConnectionType type;

  MeasurerConsumerConnectionPackerParams(
      const arguments::AnalizerArguments &arguments)
      : type(arguments.measurer_consumer_connection_type) {}
};

struct BufferParams {
  BufferParams(const arguments::AnalizerArguments &arguments) {}
};

struct MeasurerProducerConnectionServerParams {
  ConnectionType type;
  std::string id;

  MeasurerProducerConnectionServerParams(
      const arguments::AnalizerArguments &arguments)
      : type(arguments.measurer_producer_connection_type),
        id(arguments.measurer_producer_connection_id) {}
};

struct MeasurerConsumerConnectionServerParams {
  ConnectionType type;
  std::string id;

  MeasurerConsumerConnectionServerParams(
      const arguments::AnalizerArguments &arguments)
      : type(arguments.measurer_consumer_connection_type),
        id(arguments.measurer_consumer_connection_id) {}
};

DependencyAsArgument<VarianDependency<Statistics>>
CreateDependencyFromParams(const StatisticsParams &params) {
  return Statistics();
}

DependencyAsArgument<VarianDependency<Measurer>>
CreateDependencyFromParams(const MeasurerProducerParams &params) {
  return Measurer(params.chunks_total_count);
}

DependencyAsArgument<VarianDependency<Measurer>>
CreateDependencyFromParams(const MeasurerConsumerParams &params) {
  return Measurer(params.chunks_total_count);
}

DependencyAsType<
    VarianDependency<LocalPacker, NetworkPacker<Endianness::kLittle>,
                     NetworkPacker<Endianness::kBig>>>
CreateDependencyFromParams(
    const MeasurerProducerConnectionPackerParams &params) {
  switch (params.type) {
  case ConnectionType::kNetSocket: {
    const auto endianess = EndiannessChecker().Value();
    if (endianess == Endianness::kLittle) {
      return TypeHolder<NetworkPacker<Endianness::kLittle>>();
    } else if (endianess == Endianness::kBig) {
      return TypeHolder<NetworkPacker<Endianness::kBig>>();
    }
    throw std::runtime_error("Unexpected endianess type");
  } break;
  case ConnectionType::kPipe:
  case ConnectionType::kSharedMemory:
  case ConnectionType::kUnixSocket: {
    return TypeHolder<LocalPacker>();
  } break;
  }

  throw std::runtime_error("Unexpected connection type");
}

DependencyAsType<
    VarianDependency<LocalPacker, NetworkPacker<Endianness::kLittle>,
                     NetworkPacker<Endianness::kBig>>>
CreateDependencyFromParams(
    const MeasurerConsumerConnectionPackerParams &params) {
  switch (params.type) {
  case ConnectionType::kNetSocket: {
    const auto endianess = EndiannessChecker().Value();
    if (endianess == Endianness::kLittle) {
      return TypeHolder<NetworkPacker<Endianness::kLittle>>();
    } else if (endianess == Endianness::kBig) {
      return TypeHolder<NetworkPacker<Endianness::kBig>>();
    }
    throw std::runtime_error("Unexpected endianess type");
  } break;
  case ConnectionType::kPipe:
  case ConnectionType::kSharedMemory:
  case ConnectionType::kUnixSocket: {
    return TypeHolder<LocalPacker>();
  } break;
  }

  throw std::runtime_error("Unexpected connection type");
}

DependencyAsType<VarianDependency<Buffer>>
CreateDependencyFromParams(const BufferParams &params) {
  return TypeHolder<Buffer>();
}

DependencyAsArgument<VarianDependency<NetSocketServer, PipeServer,
                                      SharedMemoryServer, UnixSocketServer>>
CreateDependencyFromParams(
    const MeasurerProducerConnectionServerParams &params) {
  auto server = [](const MeasurerProducerConnectionServerParams &params)
      -> DependencyAsArgument<VarianDependency<
          NetSocketServer, PipeServer, SharedMemoryServer, UnixSocketServer>> {
    switch (params.type) {
    case ConnectionType::kNetSocket: {
      auto server = NetSocketServer(utils::ParseHostAndPort(params.id).port);
      server.Accept();
      return server;
    }
    case ConnectionType::kPipe: {
      auto server = PipeServer(params.id);
      return server;
    } break;
    case ConnectionType::kSharedMemory: {
      auto server = SharedMemoryServer(params.id);
      return server;
    }
    case ConnectionType::kUnixSocket: {
      auto server = UnixSocketServer(params.id);
      server.Accept();
      return server;
    }
    default:
      throw std::runtime_error("Unexpected connection type");
    }
  }(params);

  return server;
}

DependencyAsArgument<VarianDependency<NetSocketServer, PipeServer,
                                      SharedMemoryServer, UnixSocketServer>>
CreateDependencyFromParams(
    const MeasurerConsumerConnectionServerParams &params) {
  auto server = [](const MeasurerConsumerConnectionServerParams &params)
      -> DependencyAsArgument<VarianDependency<
          NetSocketServer, PipeServer, SharedMemoryServer, UnixSocketServer>> {
    switch (params.type) {
    case ConnectionType::kNetSocket: {
      auto server = NetSocketServer(utils::ParseHostAndPort(params.id).port);
      server.Accept();
      return server;
    }
    case ConnectionType::kPipe: {
      auto server = PipeServer(params.id);
      return server;
    } break;
    case ConnectionType::kSharedMemory: {
      auto server = SharedMemoryServer(params.id);
      return server;
    }
    case ConnectionType::kUnixSocket: {
      auto server = UnixSocketServer(params.id);
      server.Accept();
      return server;
    }
    default:
      throw std::runtime_error("Unexpected connection type");
    }
  }(params);

  return server;
}

std::unique_ptr<IAnalyzer>
CreateAnalyzer(const arguments::AnalizerArguments &arguments) {
  return ModeMaker<Interface<IAnalyzer>, Implementation<Analyzer>>::Make(
      StatisticsParams{arguments}, MeasurerConsumerParams{arguments},
      MeasurerProducerParams{arguments},
      MeasurerConsumerConnectionPackerParams{arguments},
      MeasurerProducerConnectionPackerParams{arguments},
      BufferParams{arguments},
      MeasurerConsumerConnectionServerParams{arguments},
      MeasurerProducerConnectionServerParams{arguments});
}

} // namespace

int runAsAnalyzer(int argc, char *argv[]) try {
  arguments::AnalizerArguments arguments;
  argp_parse(&arguments::definition, argc, argv, 0, 0, &arguments);

  auto analyzer = CreateAnalyzer(arguments);

  std::cout << "[analyzer] Ready to work. Working..." << std::endl
            << std::flush;

  analyzer->Run();

  std::cout << "[analyzer] Finished work" << std::endl << std::flush;

  return 0;
} catch (const std::exception &e) {
  std::cout << "[analyzer] An error occurred during work: " << e.what()
            << std::endl
            << std::flush;
  throw;
}
