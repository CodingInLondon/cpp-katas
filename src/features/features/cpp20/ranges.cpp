// C++20: Ranges library
// Composable, lazy views and pipelines over sequences.
#include <iostream>
#include <ranges>
#include <vector>

int main() {
    std::vector<int> v{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    auto pipeline = v
        | std::views::filter([](int n) { return n % 2 == 0; })
        | std::views::transform([](int n) { return n * n; });

    std::cout << "even squares: ";
    for (int n : pipeline) std::cout << n << ' ';
    std::cout << '\n';

    std::cout << "first 3 (take): ";
    for (int n : v | std::views::take(3)) std::cout << n << ' ';
    std::cout << '\n';

    std::cout << "reversed: ";
    for (int n : v | std::views::reverse) std::cout << n << ' ';
    std::cout << '\n';
    return 0;
}
