// C++11: std::unique_ptr (RAII smart pointer)
// Explicit, exclusive ownership; frees automatically, no manual delete.
#include <iostream>
#include <memory>

struct Widget {
    int id;
    explicit Widget(int i) : id(i) { std::cout << "Widget " << id << " created\n"; }
    ~Widget() { std::cout << "Widget " << id << " destroyed\n"; }
    void greet() const { std::cout << "I am widget " << id << '\n'; }
};

int main() {
    std::unique_ptr<Widget> p(new Widget(10));
    p->greet();

    // ownership transfer (unique_ptr is move-only)
    std::unique_ptr<Widget> q = std::move(p);
    std::cout << "p is " << (p ? "valid" : "null") << " after move\n";
    q->greet();
    // no delete needed: the destructor runs at scope exit
    return 0;
}
