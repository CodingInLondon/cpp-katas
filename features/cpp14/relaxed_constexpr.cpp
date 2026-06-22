// C++14: Relaxed constexpr
// constexpr functions may now use loops, local variables, and branches.
#include <iostream>

constexpr int factorial(int n) {
    int r = 1;
    for (int i = 2; i <= n; ++i) r *= i; // loop + mutation allowed since C++14
    return r;
}

constexpr int gcd(int a, int b) {
    while (b != 0) {
        int t = b;
        b = a % b;
        a = t;
    }
    return a;
}

int main() {
    constexpr int f6 = factorial(6); // computed at compile time
    static_assert(f6 == 720, "factorial(6) should be 720");
    constexpr int g = gcd(48, 36);
    static_assert(g == 12, "gcd(48,36) should be 12");

    std::cout << "6! = " << f6 << ", gcd(48,36) = " << g << '\n';
    std::cout << "runtime factorial(5) = " << factorial(5) << '\n'; // also works at runtime
    return 0;
}
