// C++11: Range-based for loops
// Clean iteration over containers and arrays without manual indices.
#include <iostream>
#include <map>
#include <string>
#include <vector>

int main() {
    std::vector<int> v{1, 2, 3, 4, 5};

    for (int x : v) std::cout << x << ' '; // read
    std::cout << '\n';

    for (int& x : v) x *= x; // modify in place via reference
    for (int x : v) std::cout << x << ' ';
    std::cout << '\n';

    int arr[] = {10, 20, 30}; // works on C arrays too
    for (int x : arr) std::cout << x << ' ';
    std::cout << '\n';

    std::map<std::string, int> m{{"a", 1}, {"b", 2}};
    for (const auto& pair : m)
        std::cout << pair.first << '=' << pair.second << ' ';
    std::cout << '\n';
    return 0;
}
