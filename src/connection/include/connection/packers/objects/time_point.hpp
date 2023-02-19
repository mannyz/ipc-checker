#pragma once

#include <chrono>

#include "connection/packers/packer_traits.hpp"

template <typename TClock, typename TDur>
struct PackerTraits<std::chrono::time_point<TClock, TDur>> {
  template <typename T>
  static void Serialize(SerializingPacker<T> &packer,
                        const std::chrono::time_point<TClock, TDur> &object) {
    packer.Serialize(object.time_since_epoch().count());
  }

  template <typename T>
  static void Deserialize(DeserializingPacker<T> &packer,
                          std::chrono::time_point<TClock, TDur> &object) {
    typename TDur::rep time_since_epoch_count;
    packer.Deserialize(time_since_epoch_count);
    object =
        std::chrono::time_point<TClock, TDur>{TDur{time_since_epoch_count}};
  }
};
