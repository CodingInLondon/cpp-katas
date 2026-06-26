// C++17: std::optional
// A type that may or may not hold a value, replacing sentinel hacks.
#include <iostream>
#include <optional>
#include <string>

std::optional<int> parse_positive(const std::string& s) {
    try {
        int n = std::stoi(s);
        if (n > 0) return n;
    } catch (...) {
    }
    return std::nullopt;
}

int main() {
    for (const std::string s : {"42", "-1", "abc"}) {
        auto r = parse_positive(s);
        if (r) std::cout << s << " -> " << *r << '\n';
        else   std::cout << s << " -> (none)\n";
    }
    std::cout << "fallback: " << parse_positive("oops").value_or(-1) << '\n';
    return 0;
}
