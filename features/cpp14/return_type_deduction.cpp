// C++14: Return-type deduction (auto return)
// The compiler deduces a function's return type from its return statements.
#include <iostream>
#include <string>

auto sum(int a, int b) { return a + b; } // deduces int

auto repeat(const std::string& s, int n) { // deduces std::string
    std::string r;
    for (int i = 0; i < n; ++i) r += s;
    return r;
}

// Recursion is fine: the first (non-recursive) return fixes the type.
auto factorial(int n) {
    if (n <= 1) return 1;
    return n * factorial(n - 1);
}

int main() {
    std::cout << sum(3, 4) << '\n';
    std::cout << repeat("ab", 3) << '\n';
    std::cout << "5! = " << factorial(5) << '\n';
    return 0;
}
