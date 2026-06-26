// C++23: if consteval
// Choose a different code path during constant evaluation vs. at runtime.
#include <cmath>
#include <iostream>

constexpr double my_pow(double base, int exp) {
    if consteval {
        // Compile-time path: a simple loop usable in constant evaluation.
        double r = 1.0;
        for (int i = 0; i < exp; ++i) r *= base;
        return r;
    } else {
        // Runtime path: defer to the optimized library function.
        return std::pow(base, exp);
    }
}

int main() {
    constexpr double ct = my_pow(2.0, 10); // evaluated at compile time
    double rt = my_pow(2.0, 10);           // evaluated at runtime
    static_assert(ct == 1024.0);

    std::cout << "compile-time my_pow(2, 10) = " << ct << '\n';
    std::cout << "runtime     my_pow(2, 10) = " << rt << '\n';
    return 0;
}
