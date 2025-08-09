#include <thread>

#include "actors/producer.hpp"
#include "arguments.hpp"
#include "run/as/common/maker.hpp"
#include "run/as/common/scheduler.hpp"
#include "run/as/common/utils.hpp"
#include "run/as/producer.hpp"

#include "connection/buffer/buffer.hpp"
#include "connection/connectors/net_socket/client.hpp"
#include "connection/connectors/net_socket/client_impl.hpp"
#include "connection/connectors/pipe/client.hpp"
#include "connection/connectors/pipe/client_impl.hpp"
#include "connection/connectors/shared_memory/client.hpp"
#include "connection/connectors/shared_memory/client_impl.hpp"
#include "connection/connectors/unix_socket/client.hpp"
#include "connection/connectors/unix_socket/client_impl.hpp"
#include "connection/packers/local_packer/local_packer.hpp"
#include "connection/packers/network_packer/network_packer.hpp"
#include "connection/packers/objects/string.hpp"
#include "connection/packers/objects/time_point.hpp"
#include "generator/simple_round_generator.hpp"
#include "generator/simple_round_generator_impl_traits.hpp"
#include "measurers/chrono/measurer.hpp"
#include "measurers/rdtsc/measurer.hpp"

namespace {

struct BufferParams {
  BufferParams(const arguments::ProducerArguments &arguments) {}
};

struct MeasurerParams {
  std::size_t chunks_total_count;

  MeasurerParams(const arguments::ProducerArguments &arguments)
      : chunks_total_count(arguments.chunks_total_count) {}
};

struct GeneratorParams {
  std::size_t chunks_size;
  std::size_t chunks_total_count;

  GeneratorParams(const arguments::ProducerArguments &arguments)
      : chunks_size(arguments.chunks_size),
        chunks_total_count(arguments.chunks_total_count) {}
};

struct ConsumerProducerConnectionClientParams {
  ConnectionType type;
  std::string id;
  std::optional<std::size_t> buffer_size;

  ConsumerProducerConnectionClientParams(
      const arguments::ProducerArguments &arguments)
      : type(arguments.connection_type), id(arguments.connection_id),
        buffer_size(arguments.connection_buffer_size) {}
};

struct ConsumerProducerConnectionPackerParams {
  ConnectionType type;

  ConsumerProducerConnectionPackerParams(
      const arguments::ProducerArguments &arguments)
      : type(arguments.connection_type) {}
};

struct MeasurerProducerConnectionClientParams {
  ConnectionType type;
  std::string id;

  MeasurerProducerConnectionClientParams(
      const arguments::ProducerArguments &arguments)
      : type(arguments.measurer_connection_type),
        id(arguments.measurer_connection_id) {}
};

struct MeasurerProducerConnectionPackerParams {
  ConnectionType type;

  MeasurerProducerConnectionPackerParams(
      const arguments::ProducerArguments &arguments)
      : type(arguments.connection_type) {}
};

DependencyAsType<VarianDependency<Buffer>>
CreateDependencyFromParams(const BufferParams &params) {
  std::cout << "[producer][packer's buffer] Capacity is "
            << Buffer::kBufferCapacityInBytes << " bytes" << std::endl
            << std::flush;
  return TypeHolder<Buffer>();
}

DependencyAsArgument<VarianDependency<RdtscMeasurer>>
CreateDependencyFromParams(const MeasurerParams &params) {
  return RdtscMeasurer(params.chunks_total_count);
}

DependencyAsArgument<VarianDependency<SimpleRoundGenerator>>
CreateDependencyFromParams(const GeneratorParams &params) {
  return SimpleRoundGenerator(params.chunks_size, params.chunks_total_count);
}

DependencyAsArgument<VarianDependency<NetSocketClient, PipeClient,
                                      SharedMemoryClient, UnixSocketClient>>
CreateDependencyFromParams(
    const ConsumerProducerConnectionClientParams &params) {
  auto client = [](const ConsumerProducerConnectionClientParams &params)
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
    default:
      throw std::runtime_error("Unexpected connection type");
    }
  }(params);

  std::visit(
      [buffer_size = params.buffer_size](auto &client) {
        while (!client.Connect()) {
          std::cout << "[producer][consumer's connector] Cannot connect to "
                    << client.GetConnectionAddress() << std::endl
                    << std::flush;
          std::this_thread::sleep_for(std::chrono::seconds{1});
        }

        std::cout << "[producer][consumer's connector] Connection "
                     "was successfully established"
                  << std::endl
                  << std::flush;

        if constexpr (requires { client.SetBufferSize(0); }) {
          if (buffer_size) {
            client.SetBufferSize(*buffer_size);
          }

          std::cout << "[producer][consumer's connector] Internal buffer "
                       "prepared for connection: actual size is "
                    << client.GetBufferSize() << " bytes, wanted size is "
                    << (buffer_size ? std::to_string(*buffer_size) + " bytes"
                                    : "--")
                    << std::endl
                    << std::flush;
        }

        if constexpr (requires { client.SetNoDelay(); }) {
          client.SetNoDelay();

          std::cout << "[producer][consumer's connector] The Nagle's algorithm "
                       "disabled for connection"
                    << std::endl
                    << std::flush;
        }
      },
      client);

  return client;
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
    const MeasurerProducerConnectionClientParams &params) {
  auto client = [](const MeasurerProducerConnectionClientParams &params)
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
    default:
      throw std::runtime_error("Unexpected connection type");
    }
  }(params);

  std::visit(
      [](auto &client) {
        while (!client.Connect()) {
          std::cout << "[producer][measurer's connector] Cannot connect to "
                    << client.GetConnectionAddress() << std::endl
                    << std::flush;
          std::this_thread::sleep_for(std::chrono::seconds{1});
        }
        std::cout << "[producer][measurer's connector] Connection was "
                     "successfully established"
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

std::unique_ptr<IProducer>
CreateProducer(const arguments::ProducerArguments &arguments) {
  return ModeMaker<Interface<IProducer>, Implementation<Producer>>::Make(
      BufferParams{arguments}, GeneratorParams{arguments},
      MeasurerParams{arguments},
      MeasurerProducerConnectionPackerParams{arguments},
      MeasurerProducerConnectionClientParams{arguments},
      ConsumerProducerConnectionPackerParams{arguments},
      ConsumerProducerConnectionClientParams{arguments});
}

} // namespace

int runAsProducer(int argc, char *argv[]) try {
  arguments::ProducerArguments arguments;
  argp_parse(&arguments::definition, argc, argv, 0, 0, &arguments);

  SchedSetAffinity(arguments.cpu);

  auto producer = CreateProducer(arguments);

  constexpr auto kWaitingTimeout = std::chrono::seconds{3};

  std::cout << "[producer] Waiting " << kWaitingTimeout.count()
            << "s for syncronization with consumer..." << std::endl
            << std::flush;

  std::this_thread::sleep_for(kWaitingTimeout);

  std::cout << "[producer] Ready to work. Working..." << std::endl
            << std::flush;

  producer->Run();

  std::cout << "[producer] Finished work" << std::endl << std::flush;

  return 0;
} catch (const std::exception &e) {
  std::cout << "[producer] An error occurred during work: " << e.what()
            << std::endl
            << std::flush;
  throw;
}
