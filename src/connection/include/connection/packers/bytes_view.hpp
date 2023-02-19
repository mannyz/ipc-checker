
#pragma once

#include <type_traits>

template <typename T> struct BytesView {
  using bytes_pointer_type = std::conditional_t<
      std::is_const<std::remove_volatile_t<std::remove_reference_t<T>>>::value,
      const char *, char *>;

  BytesView(T *pointer, std::size_t size) : pointer(pointer), size(size) {}

  bytes_pointer_type pointer;
  std::size_t size;
};
