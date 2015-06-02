#ifndef STL2_UTILITY_HPP
#define STL2_UTILITY_HPP

#include <stl2/detail/config.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/concepts/foundational.hpp>

#include <cassert>

namespace stl2 { namespace v1 {

////////////////////
// exchange and swap
//
template <Movable T, AssignableTo<T> U = T>
constexpr T exchange(T& t, U&& u)
  noexcept(std::is_nothrow_move_constructible<T>::value &&
           std::is_nothrow_assignable<T&, U>::value) {
  auto tmp = move(t);
  t = forward<U>(u);
  return tmp;
}

/*
 * http://www.open-std.org/jtc1/sc22/wg21/docs/lwg-active.html#2152
 * http://www.open-std.org/jtc1/sc22/wg21/docs/lwg-closed.html#2171
 */

template <Movable T>
constexpr void swap(T& a, T& b)
  noexcept(std::is_nothrow_move_constructible<T>::value &&
           std::is_nothrow_move_assignable<T>::value) {
  T tmp(move(a));
  a = move(b);
  b = move(tmp);
}
 
template <class T, class U, std::size_t N>
  requires detail::swappable_array<T, U>::value
constexpr void swap(T (&t)[N], U (&u)[N])
  noexcept(detail::swappable_array<T, U>::nothrow) {
  for (std::size_t i = 0; i < N; ++i) {
    swap(t[i], u[i]);
  }
}

#ifdef STL2_SWAPPABLE_POINTERS
// swap rvalue pointers
template <class T, class U>
  requires detail::SwappableLvalue<T, U>
constexpr void swap(T*&& a, U*&& b)
  noexcept(noexcept(swap(*a, *b))) {
  assert(a);
  assert(b);
  swap(*a, *b);
}

// swap rvalue pointer with lvalue reference
template <class T, class U>
  requires detail::SwappableLvalue<T, U>
constexpr void swap(T& a, U*&& b)
  noexcept(noexcept(swap(a, *b))) {
  assert(b);
  swap(a, *b);
}

template <class T, class U>
  requires detail::SwappableLvalue<T, U>
constexpr void swap(T*&& a, U& b)
  noexcept(noexcept(swap(*a, b))) {
  assert(a);
  swap(*a, b);
}
#endif // STL2_SWAPPABLE_POINTERS


////////////////////
// General Utilities
//
struct identity {
  template <class T>
  constexpr T&& operator()(T&& t) const noexcept {
    return forward<T>(t);
  }
};

template <class T = void>
  requires Same<T, void> || WeaklyEqualityComparable<T>
struct equal_to {
  constexpr auto operator()(const T& a, const T& b) const {
    return a == b;
  }
};

template <>
struct equal_to<void> {
  template <class T, WeaklyEqualityComparable<T> U>
  constexpr auto operator()(const T& t, const U& u) const {
    return t == u;
  }

  using is_transparent = std::true_type;
};

}} // namespace stl2::v1

#endif // STL2_UTILITY_HPP
