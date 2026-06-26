// C++14: Generic lambdas (auto parameters)
// One lambda becomes a templated callable handling many types.
#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

int main() {
    auto add = [](auto a, auto b) { return a + b; };
    std::cout << add(2, 3) << '\n';                      // 5 (int)
    std::cout << add(2.5, 4.5) << '\n';                  // 7 (double)
    std::cout << add(std::string("foo"), "bar") << '\n'; // foobar

    // one generic comparator usable for any container element type
    auto by_size = [](const auto& a, const auto& b) { return a.size() < b.size(); };
    std::vector<std::string> words{"ccc", "a", "bb"};
    std::sort(words.begin(), words.end(), by_size);
    for (const auto& w : words) std::cout << w << ' ';
    std::cout << '\n';
    return 0;
}
