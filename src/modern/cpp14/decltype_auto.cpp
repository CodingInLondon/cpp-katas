// C++14: decltype(auto) return type
// Deduces the return type exactly, preserving references and const.
#include <iostream>
#include <string>
#include <type_traits>

std::string& get_ref() {
    static std::string s = "hello";
    return s;
}
std::string get_val() { return "world"; }

decltype(auto) wrapper_ref() { return get_ref(); } // returns std::string&
decltype(auto) wrapper_val() { return get_val(); } // returns std::string

int main() {
    wrapper_ref() += ", world"; // modifies the static through the returned reference
    std::cout << get_ref() << '\n';

    auto v = wrapper_val();
    std::cout << v << '\n';

    static_assert(std::is_reference<decltype(wrapper_ref())>::value, "should be a reference");
    static_assert(!std::is_reference<decltype(wrapper_val())>::value, "should be a value");
    return 0;
}
