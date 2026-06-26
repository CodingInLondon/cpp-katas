// C++17: Structured bindings
// Unpack tuples, pairs, and structs into named variables.
#include <iostream>
#include <map>
#include <string>
#include <tuple>

std::tuple<int, double, std::string> make_record() {
    return {1, 9.99, "widget"};
}

int main() {
    auto [id, price, name] = make_record();
    std::cout << id << ' ' << price << ' ' << name << '\n';

    std::map<std::string, int> m{{"a", 1}, {"b", 2}};
    for (const auto& [key, value] : m)
        std::cout << key << " => " << value << '\n';

    std::pair<int, std::string> p{42, "hello"};
    auto& [num, text] = p; // bind by reference
    num = 7;
    std::cout << p.first << ' ' << text << '\n';
    return 0;
}
