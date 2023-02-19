#pragma once

#include <string>

#include "connection/packers/bytes_view.hpp"
#include "connection/packers/packer_traits.hpp"

template <> struct PackerTraits<std::string> {
  template <typename T>
  static void Serialize(SerializingPacker<T> &packer,
                        const std::string &object) {
    packer.Serialize(object.size());
    packer.Serialize(BytesView(object.c_str(), object.size()));
  }

  template <typename T>
  static void Deserialize(DeserializingPacker<T> &packer, std::string &object) {
    std::string::size_type size;
    packer.Deserialize(size);
    object.resize(size);
    packer.Deserialize(BytesView(object.data(), size));
  }
};
