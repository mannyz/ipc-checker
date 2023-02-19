#pragma once

#include "interface/interface.hpp"

INTERFACE(IBuffer)
DECLARE_METHOD(void, Reset, ());
DECLARE_METHOD(char *, Data, ());
DECLARE_METHOD(const char *, Data, (), (const));
DECLARE_METHOD(std::size_t, Capacity, (), (const));
DECLARE_METHOD(std::size_t, UsedBytes, (), (const));
DECLARE_METHOD(void, ReadBytes, (char *const data, std::size_t size));
DECLARE_METHOD(void, WriteBytes, (const char *data, std::size_t size));
DECLARE_METHOD(void, ReadBytesInReverseOrder,
               (char *const data, std::size_t size));
DECLARE_METHOD(void, WriteBytesInReverseOrder,
               (const char *data, std::size_t size));
END_INTERFACE();
