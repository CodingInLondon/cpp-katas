// C++23: std::print / std::println
// Built-in formatted output using std::format-style placeholders.
#include <print>
#include <vector>

int main() {
    std::print("Hello, {}!\n", "world");
    std::println("{0} squared is {1}", 7, 49);
    std::println("formatted: {:.2f} and {:#x}", 3.14159, 255);

    std::vector<int> v{1, 2, 3};
    std::print("vector: ");
    for (int n : v) std::print("{} ", n);
    std::println("");
    return 0;
}
