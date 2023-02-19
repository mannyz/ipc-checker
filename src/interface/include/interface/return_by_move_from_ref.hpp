#pragma once

#include <gmock/gmock-actions.h>

namespace testing::internal {

template <typename T> class ReturnByMoveFromRefAction {
public:
  // Constructs a ReturnByMoveFromRefAction object from the reference to
  // be returned.
  explicit ReturnByMoveFromRefAction(T &value) : value_(value) {} // NOLINT

  // This template type conversion operator allows ReturnByMoveFromRef(x) to be
  // used in ANY function that returns x's type.
  template <typename F> operator testing::Action<F>() const {
    typedef typename testing::internal::Function<F>::Result Result;
    // Asserts that the function return type is a not reference.
    static_assert(!std::is_reference<Result>::value,
                  "Result_cannot_be_a_reference_type");

    return testing::Action<F>(new Impl<F>(value_));
  }

private:
  // Implements the ReturnByMoveFromRefAction(x) action for a particular
  // function type F.
  template <typename F> class Impl : public testing::ActionInterface<F> {
  public:
    typedef typename testing::internal::Function<F>::Result Result;
    typedef
        typename testing::internal::Function<F>::ArgumentTuple ArgumentTuple;

    explicit Impl(T &value) : value_(value) {} // NOLINT

    Result Perform(const ArgumentTuple &) override { return std::move(value_); }

  private:
    T &value_;

  private:
    Impl(const Impl &) = delete;
  };

  T &value_;

private:
  ReturnByMoveFromRefAction(const ReturnByMoveFromRefAction &) = delete;
};

} // namespace testing::internal

namespace testing {

template <typename R>
inline internal::ReturnByMoveFromRefAction<R> ReturnByMoveFromRef(R &x) {
  return internal::ReturnByMoveFromRefAction<R>(x);
}

} // namespace testing