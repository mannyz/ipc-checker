#pragma once

#include "mock_traits.hpp"

#define DECLARE_MOCK(_Name, _MockImpl)                                         \
  MOCK_INTERNAL_ASSERT_VALID_MOCK_TYPE(_MockImpl);                             \
  struct _Name : public _MockImpl {}

#define MOCK_INTERNAL_ASSERT_VALID_MOCK_TYPE(_MockImpl)                        \
  static_assert(                                                               \
      internal::IsMock<std::decay_t<_MockImpl>>(),                             \
      "Type \'" GMOCK_PP_STRINGIZE(                                            \
          _MockImpl) "\' cannot be recognized as a Mock or its derived type.")
