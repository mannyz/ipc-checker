
#pragma once

#include "common/delay_static_assert.hpp"

#include "packer_fwd.hpp"

template <typename...> struct PackerTraits;

template <typename TObject> struct PackerTraits<TObject> {
  template <typename T>
  static void Serialize(SerializingPacker<T> &packer, const TObject &object) {
    static_assert(internal::DelayStaticAssert<TObject>, "Not implemented!");
  }

  template <typename T>
  static void Deserialize(DeserializingPacker<T> &packer, TObject &object) {
    static_assert(internal::DelayStaticAssert<TObject>, "Not implemented!");
  }
};