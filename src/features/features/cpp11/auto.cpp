// C++11: auto type deduction
// Removes repetitive type annotations; the type "tracks" the initializer.
#include <iostream>
#include <map>
#include <string>
#include <vector>

int main() {
    auto x = 42;                           // int
    auto pi = 3.14159;                     // double
    auto name = std::string{"Modern C++"}; // std::string
    auto vec = std::vector<double>{1.1, 2.2, 3.3};

    std::cout << "x = " << x << ", pi = " << pi << ", name = " << name << '\n';

    // auto shines with otherwise verbose iterator types
    std::map<std::string, int> ages{{"Alice", 30}, {"Bob", 25}};
    for (auto it = ages.begin(); it != ages.end(); ++it)
        std::cout << it->first << " is " << it->second << '\n';

    double sum = 0;
    for (auto v : vec) sum += v;
    std::cout << "sum = " << sum << '\n';
    return 0;
}
