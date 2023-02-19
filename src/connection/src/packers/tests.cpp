#include <gtest/gtest.h>

#include "connection/buffer/buffer.hpp"
#include "connection/packers/local_packer/local_packer.hpp"
#include "connection/packers/network_packer/network_packer.hpp"
#include "connection/packers/objects/string.hpp"

struct ComplexObject {
  int64_t i;
  double d;
  std::string s;
  uint64_t u;

  friend bool operator==(const ComplexObject &lhs, const ComplexObject &rhs) {
    return lhs.i == rhs.i && lhs.d == rhs.d && lhs.s == rhs.s && lhs.u == rhs.u;
  }
};

template <> struct PackerTraits<ComplexObject> {
  template <typename T>
  static void Serialize(SerializingPacker<T> &packer,
                        const ComplexObject &object) {
    packer.Serialize(object.i);
    packer.Serialize(object.d);
    packer.Serialize(object.s);
    packer.Serialize(object.u);
  }

  template <typename T>
  static void Deserialize(DeserializingPacker<T> &packer,
                          ComplexObject &object) {
    packer.Deserialize(object.i);
    packer.Deserialize(object.d);
    packer.Deserialize(object.s);
    packer.Deserialize(object.u);
  }
};

TEST(PackersTests, LocalPackerTest) {
  LocalPacker packer;
  auto buffer = IBuffer<Buffer>{};

  ComplexObject co1{.i = 123, .d = 456.789, .s = "hello_msg", .u = 654};
  packer.Pack(buffer, co1);

  ComplexObject co2;
  packer.Unpack(buffer, co2);

  EXPECT_EQ(co1, co2);
}

TEST(PackersTests, NetworkPackerBigEndiannessTest) {
  NetworkPacker<Endianness::kBig> packer;
  auto buffer = IBuffer<Buffer>{};

  ComplexObject co1{.i = 123, .d = 456.789, .s = "hello_msg", .u = 654};
  packer.Pack(buffer, co1);

  ComplexObject co2;
  packer.Unpack(buffer, co2);

  EXPECT_EQ(co1, co2);
}

TEST(PackersTests, NetworkPackerLittleEndiannessTest) {
  NetworkPacker<Endianness::kLittle> packer;
  auto buffer = IBuffer<Buffer>{};

  ComplexObject co1{.i = 123, .d = 456.789, .s = "hello_msg", .u = 654};
  packer.Pack(buffer, co1);

  ComplexObject co2;
  packer.Unpack(buffer, co2);

  EXPECT_EQ(co1, co2);
}