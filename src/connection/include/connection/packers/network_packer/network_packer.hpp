#pragma once

#include <cstdint>

#include "connection/packers/bytes_view.hpp"
#include "connection/packers/packer.hpp"
#include "interface/connection/ibuffer.hpp"

#include "endianness.hpp"

template <Endianness THostEndian> class NetworkPacker {
  template <typename TBufferImpl>
  class SerializingNetworkPacker
      : public SerializingPacker<SerializingNetworkPacker<TBufferImpl>> {
    friend SerializingPacker<SerializingNetworkPacker<TBufferImpl>>;

  public:
    SerializingNetworkPacker(IBuffer<TBufferImpl> &buffer) : _buffer(buffer) {}

  private:
    void SerializeImpl(int64_t object) {
      if constexpr (THostEndian == Endianness::kBig) {
        _buffer.WriteBytes(
            reinterpret_cast<const char *>(std::addressof(object)),
            sizeof(object));
      } else if constexpr (THostEndian == Endianness::kLittle) {
        _buffer.WriteBytesInReverseOrder(
            reinterpret_cast<const char *>(std::addressof(object)),
            sizeof(object));
      } else {
        throw std::runtime_error("Unreachable code!");
      }
    }

    void SerializeImpl(uint64_t object) {
      if constexpr (THostEndian == Endianness::kBig) {
        _buffer.WriteBytes(
            reinterpret_cast<const char *>(std::addressof(object)),
            sizeof(object));
      } else if constexpr (THostEndian == Endianness::kLittle) {
        _buffer.WriteBytesInReverseOrder(
            reinterpret_cast<const char *>(std::addressof(object)),
            sizeof(object));
      } else {
        throw std::runtime_error("Unreachable code!");
      }
    }

    void SerializeImpl(double object) {
      if constexpr (THostEndian == Endianness::kBig) {
        _buffer.WriteBytes(
            reinterpret_cast<const char *>(std::addressof(object)),
            sizeof(object));
      } else if constexpr (THostEndian == Endianness::kLittle) {
        _buffer.WriteBytesInReverseOrder(
            reinterpret_cast<const char *>(std::addressof(object)),
            sizeof(object));
      } else {
        throw std::runtime_error("Unreachable code!");
      }
    }

    template <typename T> void SerializeImpl(BytesView<T> object) {
      _buffer.WriteBytes(object.pointer, object.size);
    }

  private:
    IBuffer<TBufferImpl> &_buffer;
  };

  template <typename TBufferImpl>
  class DeserializingNetworkPacker
      : public DeserializingPacker<DeserializingNetworkPacker<TBufferImpl>> {
    friend DeserializingPacker<DeserializingNetworkPacker<TBufferImpl>>;

  public:
    DeserializingNetworkPacker(IBuffer<TBufferImpl> &buffer)
        : _buffer(buffer) {}

  private:
    void DeserializeImpl(int64_t &object) {
      if constexpr (THostEndian == Endianness::kBig) {
        _buffer.ReadBytes(reinterpret_cast<char *const>(std::addressof(object)),
                          sizeof(object));
      } else if constexpr (THostEndian == Endianness::kLittle) {
        _buffer.ReadBytesInReverseOrder(
            reinterpret_cast<char *const>(std::addressof(object)),
            sizeof(object));
      } else {
        throw std::runtime_error("Unreachable code!");
      }
    }

    void DeserializeImpl(uint64_t &object) {
      if constexpr (THostEndian == Endianness::kBig) {
        _buffer.ReadBytes(reinterpret_cast<char *const>(std::addressof(object)),
                          sizeof(object));
      } else if constexpr (THostEndian == Endianness::kLittle) {
        _buffer.ReadBytesInReverseOrder(
            reinterpret_cast<char *const>(std::addressof(object)),
            sizeof(object));
      } else {
        throw std::runtime_error("Unreachable code!");
      }
    }

    void DeserializeImpl(double &object) {
      if constexpr (THostEndian == Endianness::kBig) {
        _buffer.ReadBytes(reinterpret_cast<char *const>(std::addressof(object)),
                          sizeof(object));
      } else if constexpr (THostEndian == Endianness::kLittle) {
        _buffer.ReadBytesInReverseOrder(
            reinterpret_cast<char *const>(std::addressof(object)),
            sizeof(object));
      } else {
        throw std::runtime_error("Unreachable code!");
      }
    }

    template <typename T> void DeserializeImpl(BytesView<T> object) {
      _buffer.ReadBytes(object.pointer, object.size);
    }

  private:
    IBuffer<TBufferImpl> &_buffer;
  };

public:
  NetworkPacker() = default;

public:
  template <typename TBufferImpl, typename TObject>
  void Pack(IBuffer<TBufferImpl> &buffer, const TObject &object) {
    SerializingNetworkPacker<TBufferImpl> packer(buffer);
    packer.Serialize(object);
  }

  template <typename TBufferImpl, typename TObject>
  void Unpack(IBuffer<TBufferImpl> &buffer, TObject &object) {
    DeserializingNetworkPacker<TBufferImpl> packer(buffer);
    packer.Deserialize(object);
  }
};