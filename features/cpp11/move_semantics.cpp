// C++11: Move semantics (std::move)
// Cast to rvalue to transfer resources instead of copying them.
#include <iostream>
#include <string>
#include <utility>
#include <vector>

int main() {
    std::string a = "hello, world";
    std::string b = std::move(a); // b takes ownership of a's buffer
    std::cout << "b = " << b << '\n';
    std::cout << "a after move (valid but unspecified) = \"" << a << "\"\n";

    std::vector<std::string> words;
    std::string big(1000, 'x');
    words.push_back(std::move(big)); // moved in, no deep copy
    std::cout << "words[0] length = " << words[0].size() << '\n';
    std::cout << "big length after move = " << big.size() << '\n';
    return 0;
}
