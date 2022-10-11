#include <iostream>

#include <boost/hana/ext/std/tuple.hpp>
#include <boost/hana/zip_with.hpp>

int main() {
  // Transform tuple based on other tuple

  const auto values = std::make_tuple(10.5f, 3, 5.1);

  const std::tuple<int, unsigned char, long> types;

  auto transform = [](auto value, auto type) {
    return static_cast<decltype(type)>(value);
  };

  const auto transformed = boost::hana::zip_with(transform, values, types);

  static_assert(std::is_same_v<decltype(std::get<0>(transformed)), decltype(std::get<0>(types))>);
  static_assert(std::is_same_v<decltype(std::get<1>(transformed)), decltype(std::get<1>(types))>);
  static_assert(std::is_same_v<decltype(std::get<2>(transformed)), decltype(std::get<2>(types))>);

  std::cout << std::get<0>(transformed) << "\n";
  std::cout << std::get<1>(transformed) << "\n";
  std::cout << std::get<2>(transformed) << "\n";

  return 0;
}
