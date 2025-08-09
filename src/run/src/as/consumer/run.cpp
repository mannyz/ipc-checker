#include "actors/consumer.hpp"
#include "arguments.hpp"
#include "run/as/common/maker.hpp"
#include "run/as/common/scheduler.hpp"
#include "run/as/common/utils.hpp"
#include "run/as/consumer.hpp"

#include "connection/buffer/buffer.hpp"
#include "connection/connectors/net_socket/client.hpp"
#include "connection/connectors/net_socket/client_impl.hpp"
#include "connection/connectors/net_socket/server.hpp"
#include "connection/connectors/net_socket/server_impl.hpp"
#include "connection/connectors/pipe/client.hpp"
#include "connection/connectors/pipe/client_impl.hpp"
#include "connection/connectors/pipe/server.hpp"
#include "connection/connectors/pipe/server_impl.hpp"
#include "connection/connectors/shared_memory/client.hpp"
#include "connection/connectors/shared_memory/client_impl.hpp"
#include "connection/connectors/shared_memory/server.hpp"
#include "connection/connectors/shared_memory/server_impl.hpp"
#include "connection/connectors/unix_socket/client.hpp"
#include "connection/connectors/unix_socket/client_impl.hpp"
#include "connection/connectors/unix_socket/server.hpp"
#include "connection/connectors/unix_socket/server_impl.hpp"
#include "connection/packers/local_packer/local_packer.hpp"
#include "connection/packers/network_packer/network_packer.hpp"
#include "connection/packers/objects/string.hpp"
#include "connection/packers/objects/time_point.hpp"
#include "measurers/chrono/measurer.hpp"
#include "measurers/rdtsc/measurer.hpp"

namespace {

struct BufferParams {
  BufferParams(const arguments::ConsumerArguments &arguments) {}
};

struct MeasurerParams {
  std::size_t chunks_total_count;

  MeasurerParams(const arguments::ConsumerArguments &arguments)
      : chunks_total_count(arguments.chunks_total_count) {}
};

struct ConsumerProducerConnectionServerParams {
  ConnectionType type;
  std::string id;
  std::optional<std::size_t> buffer_size;

  ConsumerProducerConnectionServerParams(
      const arguments::ConsumerArguments &arguments)
      : type(arguments.connection_type), id(arguments.connection_id),
        buffer_size(arguments.connection_buffer_size) {}
};

struct ConsumerProducerConnectionPackerParams {
  ConnectionType type;

  ConsumerProducerConnectionPackerParams(
      const arguments::ConsumerArguments &arguments)
      : type(arguments.connection_type) {}
};

struct MeasurerConsumerConnectionClientParams {
  ConnectionType type;
  std::string id;

  MeasurerConsumerConnectionClientParams(
      const arguments::ConsumerArguments &arguments)
      : type(arguments.measurer_connection_type),
        id(arguments.measurer_connection_id) {}
};

struct MeasurerConsumerConnectionPackerParams {
  ConnectionType type;

  MeasurerConsumerConnectionPackerParams(
      const arguments::ConsumerArguments &arguments)
      : type(arguments.connection_type) {}
};

DependencyAsType<VarianDependency<Buffer>>
CreateDependencyFromParams(const BufferParams &params) {
  std::cout << "[consumer][packer's buffer] Capacity is "
            << Buffer::kBufferCapacityInBytes << " bytes" << std::endl
            << std::flush;
  return TypeHolder<Buffer>();
}

DependencyAsArgument<VarianDependency<RdtscMeasurer>>
CreateDependencyFromParams(const MeasurerParams &params) {
  return RdtscMeasurer(params.chunks_total_count);
}

DependencyAsArgument<VarianDependency<NetSocketServer, PipeServer,
                                      SharedMemoryServer, UnixSocketServer>>
CreateDependencyFromParams(
    const ConsumerProducerConnectionServerParams &params) {
  auto server = [](const ConsumerProducerConnectionServerParams &params)
      -> DependencyAsArgument<VarianDependency<
          NetSocketServer, PipeServer, SharedMemoryServer, UnixSocketServer>> {
    switch (params.type) {
    case ConnectionType::kNetSocket: {
      auto server = NetSocketServer(utils::ParseHostAndPort(params.id).port);
      server.Accept();
      if (params.buffer_size) {
        server.SetBufferSize(*params.buffer_size);
      }
      return server;
    }
    case ConnectionType::kPipe: {
      auto server = PipeServer(params.id);
      return server;
    } break;
    case ConnectionType::kSharedMemory: {
      auto server = SharedMemoryServer(params.id, params.buffer_size);
      return server;
    }
    case ConnectionType::kUnixSocket: {
      auto server = UnixSocketServer(params.id);
      server.Accept();
      return server;
    }
    }

    throw std::runtime_error("Unexpected connection type");
  }(params);

  std::visit(
      [buffer_size = params.buffer_size](const auto &server) {
        if constexpr (requires { server.GetBufferSize(); }) {
          std::cout << "[consumer][producer's connector] Internal buffer "
                       "prepared for connection: actual size is "
                    << server.GetBufferSize() << " bytes, wanted size is "
                    << (buffer_size ? std::to_string(*buffer_size) + " bytes"
                                    : "--")
                    << std::endl
                    << std::flush;
        }
      },
      server);

  return server;
}

DependencyAsType<
    VarianDependency<LocalPacker, NetworkPacker<Endianness::kLittle>,
                     NetworkPacker<Endianness::kBig>>>
CreateDependencyFromParams(
    const ConsumerProducerConnectionPackerParams &params) {
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

DependencyAsArgument<VarianDependency<NetSocketClient, PipeClient,
                                      SharedMemoryClient, UnixSocketClient>>
CreateDependencyFromParams(
    const MeasurerConsumerConnectionClientParams &params) {
  auto client = [](const MeasurerConsumerConnectionClientParams &params)
      -> DependencyAsArgument<VarianDependency<
          NetSocketClient, PipeClient, SharedMemoryClient, UnixSocketClient>> {
    switch (params.type) {
    case ConnectionType::kNetSocket: {
      auto host_and_port = utils::ParseHostAndPort(params.id);
      return NetSocketClient(host_and_port.host, host_and_port.port);
    } break;
    case ConnectionType::kPipe: {
      return PipeClient(params.id);
    } break;
    case ConnectionType::kSharedMemory: {
      return SharedMemoryClient(params.id);
    } break;
    case ConnectionType::kUnixSocket: {
      return UnixSocketClient(params.id);
    } break;
    }

    throw std::runtime_error("Unexpected connection type");
  }(params);

  std::visit(
      [](auto &client) {
        while (!client.Connect()) {
          std::cout << "[consumer][measurer's connector] Cannot connect to "
                    << client.GetConnectionAddress() << std::endl
                    << std::flush;
          std::this_thread::sleep_for(std::chrono::seconds{1});
        }
        std::cout << "[consumer][measurer's connector] Connection "
                     "was successfully established"
                  << std::endl
                  << std::flush;

        if constexpr (requires { client.SetNoDelay(); }) {
          client.SetNoDelay();
        }
      },
      client);

  return client;
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

std::unique_ptr<IConsumer>
CreateConsumer(const arguments::ConsumerArguments &arguments) {
  return ModeMaker<Interface<IConsumer>, Implementation<Consumer>>::Make(
      BufferParams{arguments}, MeasurerParams{arguments},
      MeasurerConsumerConnectionPackerParams{arguments},
      MeasurerConsumerConnectionClientParams{arguments},
      ConsumerProducerConnectionPackerParams{arguments},
      ConsumerProducerConnectionServerParams{arguments});
}

} // namespace

int runAsConsumer(int argc, char *argv[]) try {
  arguments::ConsumerArguments arguments;
  argp_parse(&arguments::definitions, argc, argv, 0, 0, &arguments);

  SchedSetAffinity(arguments.cpu);

  auto consumer = CreateConsumer(arguments);

  std::cout << "[consumer] Ready to work. Working..." << std::endl
            << std::flush;

  consumer->Run();

  std::cout << "[consumer] Finished work" << std::endl << std::flush;

  return 0;
} catch (const std::exception &e) {
  std::cout << "[consumer] An error occurred during work: " << e.what()
            << std::endl
            << std::flush;
  throw;
}
