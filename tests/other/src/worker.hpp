#include "iconnection.hpp"

class IWorker {
public:
  IWorker() = default;
  virtual ~IWorker() = default;

public:
  virtual void Run() = 0;
};

template <typename TConnectionImpl> class Worker final : public IWorker {
public:
  Worker(TConnectionImpl &&connection) : connection(std::move(connection)) {}

public:
  virtual void Run() override {
    auto connection_channel = connection.GetChannel();

    std::string msg;
    connection_channel.Read(msg);

    TimeTick tick;
    connection_channel.Write(tick);
  }

public:
  IConnection<TConnectionImpl> connection;
};