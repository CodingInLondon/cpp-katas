// C++11: Lambda functions
// Inline, anonymous function objects placed at the call site.
#include <algorithm>
#include <iostream>
#include <vector>

int main() {
    auto add = [](int x, int y) { return x + y; };
    std::cout << "add(3, 4) = " << add(3, 4) << '\n';

    int threshold = 3;
    auto above = [threshold](int n) { return n > threshold; }; // capture by value

    std::vector<int> v{5, 1, 4, 2, 3};
    std::sort(v.begin(), v.end(), [](int a, int b) { return a > b; }); // custom sort
    std::cout << "sorted desc: ";
    for (int n : v) std::cout << n << ' ';
    std::cout << '\n';

    auto count = std::count_if(v.begin(), v.end(), above);
    std::cout << "count > " << threshold << ": " << count << '\n';
    return 0;
}
