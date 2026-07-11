#include <concepts>
#include <iostream>

template <std::floating_point T>
T add_one(T x){

    return x+1;
}


template <std::integral T>
T twice(T x){
    return x*2;
}


template <typename T>
concept Numeric = std::integral<T> || std::floating_point<T>;

template<Numeric T, Numeric U>
auto midpoint(T x, U y){
    return (x+y)/2;
}

template <typename T>
concept SignedNumber = (std::integral<T> || std::floating_point<T>) && !std::unsigned_integral<T>;


template <SignedNumber T, SignedNumber U>
auto divide(T x, U y){
    return x/y;
}

int main(){
//    std::cout << add_one(41) << '\n';

    std::cout << add_one(3.14) << '\n';

    std::cout << twice(34) << '\n';

    std::cout << midpoint(2, 3.14) << '\n';

    std::cout << divide(-1, 2.5) << '\n';

    unsigned int u;

    std::cout << divide(u, 2.5) << '\n';
}

