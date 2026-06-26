// C++20: std::format
// Type-safe, Python-style text formatting with {} placeholders.
#include <format>
#include <iostream>
#include <string>

int main() {
    std::cout << std::format("Hello, {}!\n", "world");
    std::cout << std::format("{0} + {0} = {1}\n", 2, 4);
    std::cout << std::format("pi ~= {:.3f}\n", 3.14159265);
    std::cout << std::format("hex: {:#06x}\n", 255);
    std::cout << std::format("{:>8}|{:<8}|\n", "right", "left");
    return 0;
}
