#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include <gmock/gmock-spec-builders.h>

#include "interface/mock_traits.hpp"

template <typename TImpl, typename = internal::SFINAE> class ImplStore {
public:
  template <typename... TArgs>
  ImplStore(TArgs &&...args) : _impl(std::forward<TArgs>(args)...) {}

public:
  TImpl &Impl() { return _impl; }
  const TImpl &Impl() const { return _impl; }

private:
  TImpl _impl;
};

template <typename TImpl>
class ImplStore<TImpl, internal::EnableIfMock<TImpl>> : public TImpl {
private:
  struct GMockerHolderBase {
    virtual ~GMockerHolderBase() = default;
  };

  template <typename TSignature>
  struct GMockerHolder : public GMockerHolderBase {
    ::testing::internal::FunctionMocker<TSignature> mocker;
  };

public:
  template <typename... TArgs> ImplStore(TArgs &&...) {}

public:
  template <typename TSignature>
  ::testing::internal::FunctionMocker<TSignature> &
  GetMocker(const std::string &name) const {
    auto mock_it = _mocks.find(name);
    if (mock_it == std::end(_mocks)) {
      mock_it =
          _mocks.emplace(name, std::make_unique<GMockerHolder<TSignature>>())
              .first;
    }

    auto mocker_ptr =
        dynamic_cast<GMockerHolder<TSignature> *>(mock_it->second.get());
    assert(mocker_ptr != nullptr);
    return mocker_ptr->mocker;
  }

private:
  mutable std::unordered_map<std::string, std::unique_ptr<GMockerHolderBase>>
      _mocks;
};
