// C++20: Concepts
// Named, readable constraints on template parameters.
#include <concepts>
#include <iostream>
#include <string>

template <std::integral T>
T add(T a, T b) { return a + b; }

// A custom concept: any type that can be streamed to std::ostream.
template <typename T>
concept Streamable = requires(std::ostream& os, T v) {
    { os << v } -> std::same_as<std::ostream&>;
};

template <Streamable T>
void log(const T& v) { std::cout << "log: " << v << '\n'; }

int main() {
    std::cout << add(2, 3) << '\n'; // ok: ints
    // add(2.0, 3.0);               // would fail to compile: not integral
    log(42);
    log(std::string("hello concepts"));
    return 0;
}
