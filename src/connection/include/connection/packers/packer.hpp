
#pragma once

#include <type_traits>
#include <utility>

#include "packer_traits.hpp"

template <typename TImpl> class SerializingPacker {
public:
  SerializingPacker() = default;

public:
  template <typename TObject> void Serialize(TObject &&object) {
    if constexpr (requires {
                    Impl().SerializeImpl(std::forward<TObject>(object));
                  }) {
      Impl().SerializeImpl(std::forward<TObject>(object));
    } else {
      PackerTraits<std::decay_t<TObject>>::Serialize(
          *this, std::forward<TObject>(object));
    }
  }

private:
  TImpl &Impl() { return *static_cast<TImpl *>(this); }
};

template <typename TImpl> class DeserializingPacker {
public:
  DeserializingPacker() = default;

public:
  template <typename TObject> void Deserialize(TObject &&object) {
    if constexpr (requires {
                    Impl().DeserializeImpl(std::forward<TObject>(object));
                  }) {
      Impl().DeserializeImpl(std::forward<TObject>(object));
    } else {
      PackerTraits<std::decay_t<TObject>>::Deserialize(
          *this, std::forward<TObject>(object));
    }
  }

private:
  TImpl &Impl() { return *static_cast<TImpl *>(this); }
};
