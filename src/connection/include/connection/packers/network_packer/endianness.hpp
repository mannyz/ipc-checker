#pragma once

#include <cstdint>

enum class Endianness { kBig, kLittle };

union EndiannessChecker {
public:
  constexpr EndiannessChecker() noexcept : _word(0x0201u) {}

  Endianness Value() const {
    if (IsLittle()) {
      return Endianness::kLittle;
    }

    return Endianness::kBig;
  }

private:
  uint16_t _word;
  uint8_t _bytes[sizeof(decltype(_word))];

private:
  bool IsLittle() const noexcept { return _bytes[0] == 0x01u; }
};