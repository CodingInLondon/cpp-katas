// C++23: std::expected
// Holds either a value (T) or an error (E) for exception-free error handling.
#include <expected>
#include <iostream>
#include <string>

std::expected<int, std::string> parse_int(const std::string& s) {
    try {
        std::size_t pos;
        int n = std::stoi(s, &pos);
        if (pos != s.size()) return std::unexpected("trailing characters");
        return n;
    } catch (const std::exception&) {
        return std::unexpected("not a number");
    }
}

int main() {
    for (const std::string s : {"42", "12x", "abc"}) {
        auto r = parse_int(s);
        if (r) std::cout << s << " -> " << *r << '\n';
        else   std::cout << s << " -> error: " << r.error() << '\n';
    }

    // monadic chaining
    auto doubled = parse_int("21").transform([](int n) { return n * 2; });
    std::cout << "doubled: " << doubled.value_or(-1) << '\n';
    return 0;
}
