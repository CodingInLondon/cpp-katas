// C++23: Deducing this (explicit object parameter)
// One member function template handles const/non-const/rvalue *this via 'self'.
#include <iostream>
#include <string>
#include <utility>

struct Counter {
    int count = 0;
    // A single accessor that forwards the value category of the object.
    template <typename Self>
    auto&& value(this Self&& self) { return std::forward<Self>(self).count; }
};

struct Greeter {
    std::string name;
    void hello(this const Greeter& self) {
        std::cout << "Hello from " << self.name << '\n';
    }
};

int main() {
    Counter c;
    c.value() = 5; // non-const lvalue -> binds to int&
    std::cout << "count = " << c.value() << '\n';

    Greeter g{"deducing this"};
    g.hello();

    // Deducing this also enables self-referential (recursive) lambdas.
    auto fib = [](this auto&& self, int n) -> int {
        return n < 2 ? n : self(n - 1) + self(n - 2);
    };
    std::cout << "fib(10) = " << fib(10) << '\n';
    return 0;
}
