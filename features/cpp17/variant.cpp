// C++17: std::variant
// A type-safe tagged union holding one of several alternative types.
#include <iostream>
#include <string>
#include <variant>
#include <vector>

using Value = std::variant<int, double, std::string>;

struct Printer {
    void operator()(int i) const { std::cout << "int: " << i << '\n'; }
    void operator()(double d) const { std::cout << "double: " << d << '\n'; }
    void operator()(const std::string& s) const { std::cout << "string: " << s << '\n'; }
};

int main() {
    std::vector<Value> values{42, 3.14, std::string("hello")};
    for (const auto& v : values)
        std::visit(Printer{}, v);

    Value v = 10;
    std::cout << "holds int? " << std::holds_alternative<int>(v) << '\n';
    v = std::string("now a string");
    std::cout << "index = " << v.index()
              << ", value = " << std::get<std::string>(v) << '\n';
    return 0;
}
