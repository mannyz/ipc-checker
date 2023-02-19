#pragma once

#include <memory>
#include <utility>
#include <variant>

#include "common/delay_static_assert.hpp"

template <typename...> struct ModeMaker;
template <typename> struct Interface;
template <template <typename...> typename> struct Implementation;

template <typename... TImpls> using VarianDependency = std::variant<TImpls...>;

template <typename T> struct DependencyAsArgument;

template <typename T> struct DependencyAsArgumentImplWrapper : public T {
  using origin_type = T;
  DependencyAsArgumentImplWrapper(T &&origin) : T(std::move(origin)) {}
};

template <typename... TImpls>
struct DependencyAsArgument<VarianDependency<TImpls...>>
    : std::variant<DependencyAsArgumentImplWrapper<TImpls>...> {
  using std::variant<DependencyAsArgumentImplWrapper<TImpls>...>::variant;
};

template <typename T> struct DependencyAsType;

template <typename T> struct TypeHolder {};

template <typename T> struct DependencyAsTypeImplWrapper {
  using origin_type = T;
  DependencyAsTypeImplWrapper(TypeHolder<T> &&) {}
};

template <typename... TImpls>
struct DependencyAsType<VarianDependency<TImpls...>>
    : std::variant<DependencyAsTypeImplWrapper<TImpls>...> {
  using std::variant<DependencyAsTypeImplWrapper<TImpls>...>::variant;
};

template <typename TParams>
void CreateDependencyFromParams(const TParams &params) {
  static_assert(internal::DelayStaticAssert<TParams>, "Not implemented!");
}

template <typename TInterface, template <typename...> typename TImplementation>
class ModeMaker<Interface<TInterface>, Implementation<TImplementation>> {
  template <typename...> struct ModeMakerImpl;
  template <typename...> struct ParamsList;
  template <typename...> struct DependenciesList;
  template <typename...> struct ArgsList;

public:
  template <typename... TParams>
  static std::unique_ptr<TInterface> Make(TParams &&...params) {
    return ModeMakerImpl<ParamsList<TParams...>, DependenciesList<>,
                         ArgsList<>>::Make(std::forward<TParams>(params)...);
  }

  template <typename...> struct Visit;

  template <typename... TOtherParams, typename... TImpls, typename... TArgs>
  struct Visit<ParamsList<TOtherParams...>, DependenciesList<TImpls...>,
               ArgsList<TArgs...>> {

    template <typename TDependency>
    static std::unique_ptr<TInterface>
    Apply(TOtherParams &&...other_params, TArgs &&...args,
          DependencyAsArgument<TDependency> &&dependency) {
      return std::visit(
          [... other_params = std::forward<TOtherParams>(other_params),
           ... args = std::move(args)](
              auto &&dependency) mutable -> std::unique_ptr<TInterface> {
            using TImpl =
                typename std::decay_t<decltype(dependency)>::origin_type;
            return ModeMakerImpl<ParamsList<TOtherParams...>,
                                 DependenciesList<TImpls..., TImpl>,
                                 ArgsList<TArgs..., TImpl>>::
                Make(std::forward<TOtherParams>(other_params)...,
                     std::move(args)..., std::move(dependency));
          },
          std::move(dependency));
    }

    template <typename TDependency>
    static std::unique_ptr<TInterface>
    Apply(TOtherParams &&...other_params, TArgs &&...args,
          DependencyAsType<TDependency> &&dependency) {
      return std::visit(
          [... other_params = std::forward<TOtherParams>(other_params),
           ... args = std::move(args)](
              auto &&dependency) mutable -> std::unique_ptr<TInterface> {
            using TImpl =
                typename std::decay_t<decltype(dependency)>::origin_type;
            return ModeMakerImpl<ParamsList<TOtherParams...>,
                                 DependenciesList<TImpls..., TImpl>,
                                 ArgsList<TArgs...>>::
                Make(std::forward<TOtherParams>(other_params)...,
                     std::move(args)...);
          },
          std::move(dependency));
    }
  };

private:
  template <typename TCurrentParam, typename... TOtherParams,
            typename... TImpls, typename... TArgs>
  struct ModeMakerImpl<ParamsList<TCurrentParam, TOtherParams...>,
                       DependenciesList<TImpls...>, ArgsList<TArgs...>> {
    static std::unique_ptr<TInterface> Make(const TCurrentParam &current_param,
                                            TOtherParams &&...other_params,
                                            TArgs &&...args) {
      return Visit<ParamsList<TOtherParams...>, DependenciesList<TImpls...>,
                   ArgsList<TArgs...>>::
          Apply(std::forward<TOtherParams>(other_params)..., std::move(args)...,
                CreateDependencyFromParams(current_param));
    }
  };

  template <typename... TImpls, typename... TArgs>
  struct ModeMakerImpl<ParamsList<>, DependenciesList<TImpls...>,
                       ArgsList<TArgs...>> {
    static std::unique_ptr<TInterface> Make(TArgs &&...args) {
      return std::make_unique<TImplementation<TImpls...>>(std::move(args)...);
    }
  };
};
