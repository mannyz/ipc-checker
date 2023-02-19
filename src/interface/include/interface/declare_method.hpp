#pragma once

#include <gmock/gmock-function-mocker.h>

#include "mock_traits.hpp"

#define DECLARE_METHOD(...)                                                    \
  GMOCK_PP_VARIADIC_CALL(MOCK_INTERNAL_DECLARE_METHOD_ARG_, __VA_ARGS__)

#define MOCK_INTERNAL_DECLARE_METHOD_ARG_1(...)                                \
  MOCK_INTERNAL_WRONG_ARITY(__VA_ARGS__)

#define MOCK_INTERNAL_DECLARE_METHOD_ARG_2(...)                                \
  MOCK_INTERNAL_WRONG_ARITY(__VA_ARGS__)

#define MOCK_INTERNAL_DECLARE_METHOD_ARG_3(_Ret, _MethodName, _Args)           \
  MOCK_INTERNAL_DECLARE_METHOD_ARG_4(_Ret, _MethodName, _Args, ())

#define MOCK_INTERNAL_DECLARE_METHOD_ARG_4(_Ret, _MethodName, _Args, _Spec)    \
  GMOCK_INTERNAL_ASSERT_PARENTHESIS(_Args);                                    \
  GMOCK_INTERNAL_ASSERT_PARENTHESIS(_Spec);                                    \
  GMOCK_INTERNAL_ASSERT_VALID_SIGNATURE(                                       \
      GMOCK_PP_NARG0 _Args, GMOCK_INTERNAL_SIGNATURE(_Ret, _Args));            \
  GMOCK_INTERNAL_ASSERT_VALID_SPEC(_Spec);                                     \
  MOCK_INTERNAL_DECLARE_METHOD_IMPL_HELPER(                                    \
      GMOCK_PP_NARG0 _Args, _MethodName, MOCK_INTERNAL_GET_CONST_SPEC(_Spec),  \
      MOCK_INTERNAL_GET_OVERRIDE_SPEC(_Spec),                                  \
      MOCK_INTERNAL_GET_FINAL_SPEC(_Spec),                                     \
      GMOCK_INTERNAL_GET_NOEXCEPT_SPEC(_Spec),                                 \
      GMOCK_INTERNAL_GET_CALLTYPE_SPEC(_Spec),                                 \
      GMOCK_INTERNAL_GET_REF_SPEC(_Spec),                                      \
      (GMOCK_INTERNAL_SIGNATURE(_Ret, _Args)))

#define MOCK_INTERNAL_DECLARE_METHOD_ARG_5(...)                                \
  MOCK_INTERNAL_WRONG_ARITY(__VA_ARGS__)

#define MOCK_INTERNAL_DECLARE_METHOD_ARG_6(...)                                \
  MOCK_INTERNAL_WRONG_ARITY(__VA_ARGS__)

#define MOCK_INTERNAL_DECLARE_METHOD_ARG_7(...)                                \
  MOCK_INTERNAL_WRONG_ARITY(__VA_ARGS__)

#define MOCK_INTERNAL_WRONG_ARITY(...)                                         \
  static_assert(                                                               \
      false, "DECLARE_METHOD must be called with 3 or 4 arguments: _Ret, "     \
             "_MethodName, _Args and optionally _Spec. _Args and _Spec must "  \
             "be enclosed in parentheses. If _Ret is a type with unprotected " \
             "commas, it must also be enclosed in parentheses.")

#define DECLARE_TEMPLATE_METHOD(...)                                           \
  GMOCK_PP_VARIADIC_CALL(MOCK_INTERNAL_DECLARE_TEMPLATE_METHOD_ARG_,           \
                         __VA_ARGS__)

#define MOCK_INTERNAL_DECLARE_TEMPLATE_METHOD_ARG_1(...)                       \
  MOCK_INTERNAL_TEMPLATE_METHOD_WRONG_ARITY(__VA_ARGS__)

#define MOCK_INTERNAL_DECLARE_TEMPLATE_METHOD_ARG_2(...)                       \
  MOCK_INTERNAL_TEMPLATE_METHOD_WRONG_ARITY(__VA_ARGS__)

#define MOCK_INTERNAL_DECLARE_TEMPLATE_METHOD_ARG_3(...)                       \
  MOCK_INTERNAL_TEMPLATE_METHOD_WRONG_ARITY(__VA_ARGS__)

#define MOCK_INTERNAL_DECLARE_TEMPLATE_METHOD_ARG_4(_TemplateSpec, _Ret,       \
                                                    _MethodName, _Args)        \
  MOCK_INTERNAL_DECLARE_TEMPLATE_METHOD_ARG_5(_TemplateSpec, _Ret,             \
                                              _MethodName, _Args, ())

#define MOCK_INTERNAL_DECLARE_TEMPLATE_METHOD_ARG_5(_TemplateSpec, _Ret,       \
                                                    _MethodName, _Args, _Spec) \
  GMOCK_INTERNAL_ASSERT_PARENTHESIS(_Args);                                    \
  GMOCK_INTERNAL_ASSERT_PARENTHESIS(_Spec);                                    \
  GMOCK_INTERNAL_ASSERT_PARENTHESIS(_TemplateSpec);                            \
  /* TODO: add MOCK_INTERNAL_ASSERT_VALID_SIGNATURE with template respects */  \
  GMOCK_INTERNAL_ASSERT_VALID_SPEC(_Spec);                                     \
  MOCK_INTERNAL_DECLARE_TEMPLATE_METHOD_IMPL(                                  \
      GMOCK_PP_NARG0 _Args, _MethodName, MOCK_INTERNAL_GET_CONST_SPEC(_Spec),  \
      MOCK_INTERNAL_GET_OVERRIDE_SPEC(_Spec),                                  \
      MOCK_INTERNAL_GET_FINAL_SPEC(_Spec),                                     \
      GMOCK_INTERNAL_GET_NOEXCEPT_SPEC(_Spec),                                 \
      GMOCK_INTERNAL_GET_CALLTYPE_SPEC(_Spec),                                 \
      GMOCK_INTERNAL_GET_REF_SPEC(_Spec),                                      \
      (GMOCK_INTERNAL_SIGNATURE(_Ret, _Args)), _TemplateSpec)

#define MOCK_INTERNAL_DECLARE_TEMPLATE_METHOD_ARG_6(...)                       \
  MOCK_INTERNAL_TEMPLATE_METHOD_WRONG_ARITY(__VA_ARGS__)

#define MOCK_INTERNAL_DECLARE_TEMPLATE_METHOD_ARG_7(...)                       \
  MOCK_INTERNAL_TEMPLATE_METHOD_WRONG_ARITY(__VA_ARGS__)

#define MOCK_INTERNAL_TEMPLATE_METHOD_WRONG_ARITY(...)                         \
  static_assert(                                                               \
      false, "DECLARE_TEMPLATE_METHOD must be called with 4 or 5 arguments: "  \
             "_TemplateSpec, _Ret, _MethodName, _Args and optionally _Spec. "  \
             "_Args, _Spec and _TemplateSpec must be enclosed in "             \
             "parentheses. If _Ret is a type with "                            \
             "unprotected commas, it must also be enclosed in parentheses.")

// TODO: remove dependency from internal gmock implementation
#define MOCK_INTERNAL_DECLARE_METHOD_IMPL_HELPER(                              \
    _N, _MethodName, _ConstSpec, _OverrideSpec, _FinalSpec, _NoexceptSpec,     \
    _CallType, _RefSpec, _Signature)                                           \
public:                                                                        \
  template <typename T = TImpl,                                                \
            internal::EnableIfNotMock<T> = internal::kSfinae>                  \
  typename ::testing::internal::Function<GMOCK_PP_REMOVE_PARENS(               \
      _Signature)>::Result                                                     \
  GMOCK_INTERNAL_EXPAND(_CallType)                                             \
      _MethodName(GMOCK_PP_REPEAT(GMOCK_INTERNAL_PARAMETER, _Signature, _N))   \
          _ConstSpec _RefSpec _NoexceptSpec _OverrideSpec _FinalSpec {         \
    return _impl_store.Impl()._MethodName(                                     \
        GMOCK_PP_REPEAT(GMOCK_INTERNAL_FORWARD_ARG, _Signature, _N));          \
  }                                                                            \
  template <typename T = TImpl, internal::EnableIfMock<T> = internal::kSfinae> \
  typename ::testing::internal::Function<GMOCK_PP_REMOVE_PARENS(               \
      _Signature)>::Result                                                     \
  GMOCK_INTERNAL_EXPAND(_CallType)                                             \
      _MethodName(GMOCK_PP_REPEAT(GMOCK_INTERNAL_PARAMETER, _Signature, _N))   \
          _ConstSpec _RefSpec _NoexceptSpec _OverrideSpec _FinalSpec {         \
    auto &mocker =                                                             \
        _impl_store.template GetMocker<GMOCK_PP_REMOVE_PARENS(_Signature)>(    \
            GMOCK_PP_STRINGIZE(GMOCK_MOCKER_(_N, _ConstSpec, _MethodName)));   \
    mocker.SetOwnerAndName(this, #_MethodName);                                \
    return mocker.Invoke(                                                      \
        GMOCK_PP_REPEAT(GMOCK_INTERNAL_FORWARD_ARG, _Signature, _N));          \
  }                                                                            \
  template <typename T = TImpl, internal::EnableIfMock<T> = internal::kSfinae> \
  ::testing::MockSpec<GMOCK_PP_REMOVE_PARENS(_Signature)> gmock_##_MethodName( \
      GMOCK_PP_REPEAT(GMOCK_INTERNAL_MATCHER_PARAMETER, _Signature, _N))       \
      _ConstSpec _RefSpec {                                                    \
    auto &mocker =                                                             \
        _impl_store.template GetMocker<GMOCK_PP_REMOVE_PARENS(_Signature)>(    \
            GMOCK_PP_STRINGIZE(GMOCK_MOCKER_(_N, _ConstSpec, _MethodName)));   \
    mocker.RegisterOwner(this);                                                \
    return mocker.With(                                                        \
        GMOCK_PP_REPEAT(GMOCK_INTERNAL_MATCHER_ARGUMENT, , _N));               \
  }                                                                            \
  template <typename T = TImpl, internal::EnableIfMock<T> = internal::kSfinae> \
  ::testing::MockSpec<GMOCK_PP_REMOVE_PARENS(_Signature)> gmock_##_MethodName( \
      const ::testing::internal::WithoutMatchers &,                            \
      _ConstSpec ::testing::internal::Function<GMOCK_PP_REMOVE_PARENS(         \
          _Signature)> *) const _RefSpec _NoexceptSpec {                       \
    return ::testing::internal::ThisRefAdjuster<                               \
               _ConstSpec int _RefSpec>::Adjust(*this)                         \
        .gmock_##_MethodName(GMOCK_PP_REPEAT(                                  \
            GMOCK_INTERNAL_A_MATCHER_ARGUMENT, _Signature, _N));               \
  }

// TODO: remove dependency from internal gmock implementation
// TODO: add universal reference support
#define MOCK_INTERNAL_DECLARE_TEMPLATE_METHOD_IMPL(                            \
    _N, _MethodName, _ConstSpec, _OverrideSpec, _FinalSpec, _NoexceptSpec,     \
    _CallType, _RefSpec, _Signature, _TemplateSpec)                            \
public:                                                                        \
  template <MOCK_INTERNAL_MATCHER_TEMPLATE_PARAMS(_TemplateSpec),              \
            typename T = TImpl,                                                \
            internal::EnableIfNotMock<T> = internal::kSfinae>                  \
  typename ::testing::internal::Function<GMOCK_PP_REMOVE_PARENS(               \
      _Signature)>::Result                                                     \
  GMOCK_INTERNAL_EXPAND(_CallType)                                             \
      _MethodName(GMOCK_PP_REPEAT(GMOCK_INTERNAL_PARAMETER, _Signature, _N))   \
          _ConstSpec _RefSpec _NoexceptSpec _OverrideSpec _FinalSpec {         \
    return _impl_store.Impl()                                                  \
        .template _MethodName<MOCK_INTERNAL_MATCHER_TEMPLATE_ARGS(             \
            _TemplateSpec)>(                                                   \
            GMOCK_PP_REPEAT(GMOCK_INTERNAL_FORWARD_ARG, _Signature, _N));      \
  }                                                                            \
  template <MOCK_INTERNAL_MATCHER_TEMPLATE_PARAMS(_TemplateSpec),              \
            typename T = TImpl, internal::EnableIfMock<T> = internal::kSfinae> \
  typename ::testing::internal::Function<GMOCK_PP_REMOVE_PARENS(               \
      _Signature)>::Result                                                     \
  GMOCK_INTERNAL_EXPAND(_CallType)                                             \
      _MethodName(GMOCK_PP_REPEAT(GMOCK_INTERNAL_PARAMETER, _Signature, _N))   \
          _ConstSpec _RefSpec _NoexceptSpec _OverrideSpec _FinalSpec {         \
    auto &mocker =                                                             \
        _impl_store.template GetMocker<GMOCK_PP_REMOVE_PARENS(_Signature)>(    \
            GMOCK_PP_STRINGIZE(GMOCK_MOCKER_(_N, _ConstSpec, _MethodName)));   \
    mocker.SetOwnerAndName(this, #_MethodName);                                \
    return mocker.Invoke(                                                      \
        GMOCK_PP_REPEAT(GMOCK_INTERNAL_FORWARD_ARG, _Signature, _N));          \
  }                                                                            \
  template <MOCK_INTERNAL_MATCHER_TEMPLATE_PARAMS(_TemplateSpec),              \
            typename T = TImpl, internal::EnableIfMock<T> = internal::kSfinae> \
  ::testing::MockSpec<GMOCK_PP_REMOVE_PARENS(_Signature)> gmock_##_MethodName( \
      GMOCK_PP_REPEAT(GMOCK_INTERNAL_MATCHER_PARAMETER, _Signature, _N))       \
      _ConstSpec _RefSpec {                                                    \
    auto &mocker =                                                             \
        _impl_store.template GetMocker<GMOCK_PP_REMOVE_PARENS(_Signature)>(    \
            GMOCK_PP_STRINGIZE(GMOCK_MOCKER_(_N, _ConstSpec, _MethodName)));   \
    mocker.RegisterOwner(this);                                                \
    return mocker.With(                                                        \
        GMOCK_PP_REPEAT(GMOCK_INTERNAL_MATCHER_ARGUMENT, , _N));               \
  }                                                                            \
  template <MOCK_INTERNAL_MATCHER_TEMPLATE_PARAMS(_TemplateSpec),              \
            typename T = TImpl, internal::EnableIfMock<T> = internal::kSfinae> \
  ::testing::MockSpec<GMOCK_PP_REMOVE_PARENS(_Signature)> gmock_##_MethodName( \
      const ::testing::internal::WithoutMatchers &,                            \
      _ConstSpec ::testing::internal::Function<GMOCK_PP_REMOVE_PARENS(         \
          _Signature)> *) const _RefSpec _NoexceptSpec {                       \
    return ::testing::internal::ThisRefAdjuster<                               \
               _ConstSpec int _RefSpec>::Adjust(*this)                         \
        .template gmock_##_MethodName<MOCK_INTERNAL_MATCHER_TEMPLATE_ARGS(     \
            _TemplateSpec)>(GMOCK_PP_REPEAT(GMOCK_INTERNAL_A_MATCHER_ARGUMENT, \
                                            _Signature, _N));                  \
  }

#define MOCK_INTERNAL_GET_CONST_SPEC(_Tuple)                                   \
  GMOCK_PP_FOR_EACH(MOCK_INTERNAL_CONST_SPEC_IF_CONST, ~, _Tuple)

#define MOCK_INTERNAL_CONST_SPEC_IF_CONST(_i, _, _elem)                        \
  GMOCK_PP_IF(GMOCK_PP_HAS_COMMA(GMOCK_INTERNAL_DETECT_CONST(_i, _, _elem)),   \
              _elem, )

#define MOCK_INTERNAL_GET_OVERRIDE_SPEC(_Tuple)                                \
  GMOCK_PP_FOR_EACH(MOCK_INTERNAL_OVERRIDE_SPEC_IF_OVERRIDE, ~, _Tuple)

#define MOCK_INTERNAL_OVERRIDE_SPEC_IF_OVERRIDE(_i, _, _elem)                  \
  GMOCK_PP_IF(                                                                 \
      GMOCK_PP_HAS_COMMA(GMOCK_INTERNAL_DETECT_OVERRIDE(_i, _, _elem)),        \
      _elem, )

#define MOCK_INTERNAL_GET_FINAL_SPEC(_Tuple)                                   \
  GMOCK_PP_FOR_EACH(MOCK_INTERNAL_FINAL_SPEC_IF_FINAL, ~, _Tuple)

#define MOCK_INTERNAL_FINAL_SPEC_IF_FINAL(_i, _, _elem)                        \
  GMOCK_PP_IF(GMOCK_PP_HAS_COMMA(GMOCK_INTERNAL_DETECT_FINAL(_i, _, _elem)),   \
              _elem, )

#define MOCK_INTERNAL_MATCHER_TEMPLATE_PARAMS(_Tuple)                          \
  GMOCK_PP_TAIL(                                                               \
      GMOCK_PP_FOR_EACH(MOCK_INTERNAL_MATCHER_TEMPLATE_PARAM, _, _Tuple))

#define MOCK_INTERNAL_MATCHER_TEMPLATE_PARAM(_i_unused, _data_unused, _elem)   \
  , typename _elem

#define MOCK_INTERNAL_MATCHER_TEMPLATE_ARGS(_Tuple)                            \
  GMOCK_PP_TAIL(                                                               \
      GMOCK_PP_FOR_EACH(MOCK_INTERNAL_MATCHER_TEMPLATE_ARG, _, _Tuple))

#define MOCK_INTERNAL_MATCHER_TEMPLATE_ARG(_i_unused, _data_unused, _elem)     \
  , _elem

#define ON_CALL_TEMPLATE(obj, template_spec, method, call)                     \
  GMOCK_INTERNAL_ASSERT_PARENTHESIS(template_spec);                            \
  GMOCK_INTERNAL_ASSERT_PARENTHESIS(call);                                     \
  MOCK_ON_CALL_TEMPLATE_IMPL_(obj, InternalDefaultActionSetAt, template_spec,  \
                              method, call)

#define EXPECT_CALL_TEMPLATE(obj, template_spec, method, call)                 \
  GMOCK_INTERNAL_ASSERT_PARENTHESIS(template_spec);                            \
  GMOCK_INTERNAL_ASSERT_PARENTHESIS(call);                                     \
  MOCK_ON_CALL_TEMPLATE_IMPL_(obj, InternalExpectedAt, template_spec, method,  \
                              call)

#define MOCK_ON_CALL_TEMPLATE_IMPL_(mock_expr, Setter, template_spec, method,  \
                                    call)                                      \
  ((mock_expr)                                                                 \
       .gmock_##method<MOCK_INTERNAL_MATCHER_TEMPLATE_ARGS(template_spec)>     \
           call)(::testing::internal::GetWithoutMatchers(), nullptr)           \
      .Setter(__FILE__, __LINE__, #mock_expr,                                  \
              #method                                                          \
              "<" GMOCK_PP_STRINGIZE(MOCK_INTERNAL_MATCHER_TEMPLATE_ARGS(      \
                  template_spec)) ">" #call)
