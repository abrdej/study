#include <iostream>
#include <optional>
#include <variant>

// Playing with std::variant based parser idea.

namespace detail {

template <typename T>
void Print(const T& x);

template <>
void Print<int>(const int& x) {
  std::cout << "This is integer: " << x << "\n";
}

template <>
void Print<std::string>(const std::string& x) {
  std::cout << "This is string: " << x << "\n";
}

template <>
void Print<bool>(const bool& x) {
  std::cout << "This is boolean: " << (x ? "true" : "false") << "\n";
}

template <typename V, typename T>
struct Converter;

template <typename V>
struct Converter<V, int> {
  void operator()(std::optional<V>& x, const std::string& data) {
    try {
      x = std::stoi(data);
    } catch (std::invalid_argument&) {}
  }
};

template <typename V>
struct Converter<V, bool> {
  void operator()(std::optional<V>& x, const std::string& data) {
    if (data == "true") {
      x = true;
    } else if (data == "false") {
      x = false;
    }
  }
};

template <typename V>
struct Converter<V, std::string> {
  void operator()(std::optional<V>& x, const std::string& data) {
    if (!data.empty()) {
      x = data;
    }
  }
};

template <typename V, typename T>
void ConvertIf(std::optional<V>& x, const std::string& data) {
  if (!x) {
    Converter<V, T>{}(x, data);
  }
}

template <typename... Ts>
void Parse(std::optional<std::variant<Ts...>>& x, const std::string& data) {
  (ConvertIf<std::variant<Ts...>, Ts>(x, data), ...);
}

}  // namespace detail

template <typename... Ts>
struct Parsed {
  using Variant = std::variant<Ts...>;

  template <typename T>
  operator T() {
    try {
      return std::get<T>(data);
    } catch (std::bad_variant_access& e) {
      throw std::invalid_argument("Conversion not possible!");
    }
  }

  Variant data;
};

template <typename P>
void PrintParsed(const P& x) {
  std::visit([](const auto& arg) {
    detail::Print<std::decay_t<decltype(arg)>>(arg);
  }, x.data);
}

template <typename P>
P Parse(const std::string& data) {
  std::optional<typename P::Variant> variant;
  detail::Parse(variant, data);
  if (!variant) {
    throw std::invalid_argument("Parsing error!");
  }
  return {std::move(*variant)};
}

int main() {
  using ParsedT = Parsed<int, bool, std::string>;

  std::string data1 = "10";
  std::string data2 = "false";
  std::string data3 = "Text";

  auto v1 = Parse<ParsedT>(data1);
  auto v2 = Parse<ParsedT>(data2);
  auto v3 = Parse<ParsedT>(data3);

  int x1 = v1;
  std::cout << "Int: " << x1 << "\n";

  bool x2 = v2;
  std::cout << "Boolean: " << x2 << "\n";

  std::string x3 = v3;
  std::cout << "String: " << x3 << "\n";

  try {
    std::string x4 = v1;
    (void)x4;

  } catch (std::invalid_argument& e) {
    std::cout << "v1 is not string!\n";
  }

  PrintParsed(v1);
  PrintParsed(v2);
  PrintParsed(v3);

  return 0;
}
