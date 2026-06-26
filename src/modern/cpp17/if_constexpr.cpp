// C++17: if constexpr
// Compile-time branch selection; only the taken branch is instantiated.
#include <iostream>
#include <string>
#include <type_traits>

template <typename T>
std::string describe(T t) {
    if constexpr (std::is_integral_v<T>)
        return "integral: " + std::to_string(t);
    else if constexpr (std::is_floating_point_v<T>)
        return "floating: " + std::to_string(t);
    else
        return "other: " + std::string(t);
}

int main() {
    std::cout << describe(42) << '\n';
    std::cout << describe(3.14) << '\n';
    std::cout << describe("text") << '\n';
    return 0;
}
