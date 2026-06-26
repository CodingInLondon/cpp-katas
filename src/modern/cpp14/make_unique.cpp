// C++14: std::make_unique
// Exception-safe factory for unique_ptr; no naked new.
#include <iostream>
#include <memory>
#include <string>

struct Foo {
    std::string name;
    int value;
    Foo(std::string n, int v) : name(std::move(n)), value(v) {}
};

int main() {
    auto ptr = std::make_unique<Foo>("answer", 42);
    std::cout << ptr->name << " = " << ptr->value << '\n';

    auto arr = std::make_unique<int[]>(5); // array form
    for (int i = 0; i < 5; ++i) arr[i] = i * i;
    for (int i = 0; i < 5; ++i) std::cout << arr[i] << ' ';
    std::cout << '\n';
    return 0;
}
