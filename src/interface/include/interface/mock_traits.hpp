#pragma once

#include <type_traits>

#include "mock.hpp"
#include "sfinae.hpp"

namespace internal {

std::true_type MockProbe(const Mock &);
std::false_type MockProbe(...);

template <typename T> constexpr bool IsMock() {
  return decltype(MockProbe(std::declval<const T &>()))::value;
}

template <typename T>
using EnableIfMock =
    typename std::enable_if<internal::IsMock<T>(), internal::SFINAE>::type;

template <typename T>
using EnableIfNotMock =
    typename std::enable_if<!internal::IsMock<T>(), internal::SFINAE>::type;

} // namespace internal
