// C++20: Modules
// A consumer that imports the 'greet' module instead of including a header.
//
// Named modules need the interface unit (greet.cppm) compiled first, so this
// sample builds in two steps (GCC):
//   g++-14 -std=c++20 -fmodules-ts -c -x c++ greet.cppm -o greet.o
//   g++-14 -std=c++20 -fmodules-ts modules.cpp greet.o -o modules
// (build.sh does this automatically.)
#include <iostream>
import greet;

int main() {
    std::cout << "add(2, 3)      = " << add(2, 3) << '\n';
    std::cout << "multiply(4, 5) = " << multiply(4, 5) << '\n';
    return 0;
}
