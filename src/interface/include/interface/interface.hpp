#pragma once

#include "declare_method.hpp"
#include "impl_store.hpp"

#define INTERFACE(_Name)                                                       \
  template <typename TImpl> class _Name final {                                \
  private:                                                                     \
    ImplStore<TImpl> _impl_store;                                              \
                                                                               \
  public:                                                                      \
    template <typename... TArgs>                                               \
    _Name(TArgs &&...args) : _impl_store(std::forward<TArgs>(args)...) {}

#define END_INTERFACE() }
