#include <vector>
#include <string>
#include <iostream>
#include <memory>

struct Foo{
    Foo(int i):i(0){

    }

    int i;
};


// C++ 14. 3
auto sum(int a, int b) { 
    return a + b; 
} // deduces return type int


// C++ 14. 4
constexpr int factorial(int n) {
    int r = 1;
    for (int i = 1; i <= n; ++i) r *= i;
    return r;
}
constexpr int f6 = factorial(6); // computed at compile-time


// C++ 14. 5
std::string& get1() { static std::string s = "hello"; return s; }
std::string get2() { return "world"; }
decltype(auto) wrapper1() { return get1(); } // returns std::string&
decltype(auto) wrapper2() { return get2(); } // returns std::string



int main(){

    // C++11
    
    // 1. auto
    auto x = 42; // x is int
    auto vec = std::vector<double>{1.1, 2.2};

    // 2. move semantics
    std::string a = "hello";
    std::string b = std::move(a); // b takes ownership; a is valid but its value is unspecified

    // 3. lambda
    auto add = [](int x, int y) { return x + y; };
    std::cout << add(3, 4); // outputs 7
    
    // 4. unique_ptr
    struct MyClass {
        explicit MyClass(int) {}
    };

    std::unique_ptr<MyClass> p(new MyClass(10));

    std::cout << "\n";    

    // 5. ranges2
    std::vector<int> v = {1,2,3};
    for (int x : v) {
        std::cout << x << "\n"; // prints 1 2 3
    }    

    

    // C++14
    // 1. generic lambdas
    auto f = [](auto a, auto b) { return a + b; };
    std::cout << "C++14\n";
    std::cout << f(2, 3) << f(2.5, 4.5); // outputs 5 and 7
    std::cout << "\n";

    // 2. std::make_unique
    std::unique_ptr<Foo> ptr = std::make_unique<Foo>(1);

    // 3. Return-type deduction
    std::cout << sum(1.0,2.0);



    return 1;
}