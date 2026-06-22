// C++17: std::string_view
// Non-owning, read-only view of a character sequence; zero-copy.
#include <iostream>
#include <string>
#include <string_view>

void print_sv(std::string_view sv) {
    std::cout << '[' << sv << "] len=" << sv.size() << '\n';
}

// Count words without allocating any substring.
std::size_t word_count(std::string_view text) {
    std::size_t count = 0, i = 0;
    while (i < text.size()) {
        while (i < text.size() && text[i] == ' ') ++i;
        if (i < text.size()) {
            ++count;
            while (i < text.size() && text[i] != ' ') ++i;
        }
    }
    return count;
}

int main() {
    print_sv("string literal"); // no allocation
    std::string owned = "a heap string";
    print_sv(owned);            // no copy
    print_sv(std::string_view(owned).substr(2, 4)); // substring view, still no allocation

    std::cout << "words: " << word_count("the quick brown fox") << '\n';
    return 0;
}
