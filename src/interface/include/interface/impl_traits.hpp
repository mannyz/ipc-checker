#pragma once

#include <gmock/gmock-function-mocker.h>

template <typename TImpl> struct ImplTraits;
template <typename TTag> struct ForTag;

template <typename TImpl, typename TFor, typename TForGccBuildOnly = void>
using ImplTraitsFor =
    typename ImplTraits<TImpl>::template For<TFor, TForGccBuildOnly>;

template <typename TImpl, typename TFor>
using ImplTraitsFor_T = typename ImplTraitsFor<TImpl, TFor>::Type;

#define USING_IMPL_FOR_TAG(_Name, _Tag)                                        \
  using _Name = ImplTraitsFor_T<TImpl, ForTag<_Tag>>

#define IMPL_TRAITS_DEFINITION(_Name)                                          \
  template <> struct ImplTraits<_Name> {                                       \
    template <typename TFor, typename TForGccBuildOnly> struct For;

#define USE_IMPL_FOR_TAG(_Tag, _Impl)                                          \
  template <typename TForGccBuildOnly>                                         \
  struct For<ForTag<_Tag>, TForGccBuildOnly> {                                 \
    using Type = _Impl;                                                        \
  };

#define END_IMPL_TRAITS_DEFINITION() }

#define IMPL_TRAITS_TEMPLATE_DEFINITION(_TemplateSpec, _Name)                  \
  GMOCK_INTERNAL_ASSERT_PARENTHESIS(_TemplateSpec);                            \
  template <IMPL_TRAITS_INTERNAL_TEMPLATE_PARAMS(_TemplateSpec)>               \
  struct ImplTraits<                                                           \
      _Name<IMPL_TRAITS_INTERNAL_TEMPLATE_ARGS(_TemplateSpec)>> {              \
    template <template <typename> typename TFor, typename TForGccBuildOnly>    \
    struct For;

#define END_IMPL_TRAITS_TEMPLATE_DEFINITION() }

#define IMPL_TRAITS_INTERNAL_TEMPLATE_PARAMS(_Tuple)                           \
  GMOCK_PP_TAIL(                                                               \
      GMOCK_PP_FOR_EACH(IMPL_TRAITS_INTERNAL_TEMPLATE_PARAM, _, _Tuple))

#define IMPL_TRAITS_INTERNAL_TEMPLATE_PARAM(_i_unused, _data_unused, _elem)    \
  , typename _elem

#define IMPL_TRAITS_INTERNAL_TEMPLATE_ARGS(_Tuple)                             \
  GMOCK_PP_TAIL(GMOCK_PP_FOR_EACH(IMPL_TRAITS_INTERNAL_TEMPLATE_ARG, _, _Tuple))

#define IMPL_TRAITS_INTERNAL_TEMPLATE_ARG(_i_unused, _data_unused, _elem)      \
  , _elem