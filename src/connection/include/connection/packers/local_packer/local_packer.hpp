#pragma once

#include <cstdint>

#include "connection/packers/bytes_view.hpp"
#include "connection/packers/packer.hpp"
#include "interface/connection/ibuffer.hpp"

class LocalPacker {
  template <typename TBufferImpl>
  class SerializingLocalPacker
      : public SerializingPacker<SerializingLocalPacker<TBufferImpl>> {
    friend SerializingPacker<SerializingLocalPacker<TBufferImpl>>;

  public:
    SerializingLocalPacker(IBuffer<TBufferImpl> &buffer) : _buffer(buffer) {}

  private:
    void SerializeImpl(int64_t object) {
      _buffer.WriteBytes(reinterpret_cast<const char *>(std::addressof(object)),
                         sizeof(object));
    }

    void SerializeImpl(uint64_t object) {
      _buffer.WriteBytes(reinterpret_cast<const char *>(std::addressof(object)),
                         sizeof(object));
    }

    void SerializeImpl(double object) {
      _buffer.WriteBytes(reinterpret_cast<const char *>(std::addressof(object)),
                         sizeof(object));
    }

    template <typename T> void SerializeImpl(BytesView<T> object) {
      _buffer.WriteBytes(object.pointer, object.size);
    }

  private:
    IBuffer<TBufferImpl> &_buffer;
  };

  template <typename TBufferImpl>
  class DeserializingLocalPacker
      : public DeserializingPacker<DeserializingLocalPacker<TBufferImpl>> {
    friend DeserializingPacker<DeserializingLocalPacker<TBufferImpl>>;

  public:
    DeserializingLocalPacker(IBuffer<TBufferImpl> &buffer) : _buffer(buffer) {}

  private:
    void DeserializeImpl(int64_t &object) {
      _buffer.ReadBytes(reinterpret_cast<char *const>(std::addressof(object)),
                        sizeof(object));
    }

    void DeserializeImpl(uint64_t &object) {
      _buffer.ReadBytes(reinterpret_cast<char *const>(std::addressof(object)),
                        sizeof(object));
    }

    void DeserializeImpl(double &object) {
      _buffer.ReadBytes(reinterpret_cast<char *const>(std::addressof(object)),
                        sizeof(object));
    }

    template <typename T> void DeserializeImpl(BytesView<T> object) {
      _buffer.ReadBytes(object.pointer, object.size);
    }

  private:
    IBuffer<TBufferImpl> &_buffer;
  };

public:
  LocalPacker() = default;

public:
  template <typename TBufferImpl, typename TObject>
  void Pack(IBuffer<TBufferImpl> &buffer, const TObject &object) {
    SerializingLocalPacker<TBufferImpl> packer(buffer);
    packer.Serialize(object);
  }

  template <typename TBufferImpl, typename TObject>
  void Unpack(IBuffer<TBufferImpl> &buffer, TObject &object) {
    DeserializingLocalPacker<TBufferImpl> packer(buffer);
    packer.Deserialize(object);
  }
};
