// Forwarding references (a.k.a. universal references): T&&  in a deduced context.
//
// Key rule — reference collapsing during template argument deduction:
//   - Pass an lvalue of type X → T is deduced as X&,  so T&& collapses to X&   (lvalue ref)
//   - Pass an rvalue of type X → T is deduced as X,   so T&& stays as    X&&  (rvalue ref)
//
// This is what lets `std::forward<T>` preserve the original value category when
// relaying an argument to another function.

#include <iostream>
#include <string>
#include <type_traits>
#include <utility>

// ── Inspector: reports the deduced T and the resulting parameter type ─────────
template<typename T>
void inspect(T&& arg) {
    std::cout << "  T is "
              << (std::is_lvalue_reference_v<T> ? "lvalue-ref" :
                  std::is_rvalue_reference_v<T> ? "rvalue-ref" : "value")
              << "   |   T&& is "
              << (std::is_lvalue_reference_v<decltype(arg)> ? "lvalue-ref" : "rvalue-ref")
              << '\n';
}

// ── Sink overloads that react to value category ──────────────────────────────
void sink(const std::string&) { std::cout << "    sink(lvalue)\n"; }
void sink(std::string&&)      { std::cout << "    sink(rvalue)\n"; }

// ── Forwarder: uses std::forward<T> to preserve the original value category ──
template<typename T>
void forwarder(T&& arg) {
    sink(std::forward<T>(arg));
}

// ── Contrast: without std::forward, the named parameter is always an lvalue ──
template<typename T>
void broken_forwarder(T&& arg) {
    sink(arg);  // 'arg' is a named variable → lvalue, regardless of T
}

int main() {
    std::string s = "hello";

    std::cout << "inspect(s)            // lvalue argument\n";
    inspect(s);

    std::cout << "inspect(std::move(s)) // rvalue argument\n";
    inspect(std::move(s));

    std::cout << "inspect(std::string{\"tmp\"})\n";
    inspect(std::string{"tmp"});

    std::cout << "\n-- forwarder preserves category --\n";
    std::string a = "lval";
    std::cout << "forwarder(a):\n";
    forwarder(a);                       // → sink(lvalue)
    std::cout << "forwarder(std::move(a)):\n";
    forwarder(std::move(a));            // → sink(rvalue)

    std::cout << "\n-- broken_forwarder drops rvalue-ness --\n";
    std::string b = "lval";
    std::cout << "broken_forwarder(b):\n";
    broken_forwarder(b);                // → sink(lvalue)
    std::cout << "broken_forwarder(std::move(b)):\n";
    broken_forwarder(std::move(b));     // → sink(lvalue)  ← the bug
}
