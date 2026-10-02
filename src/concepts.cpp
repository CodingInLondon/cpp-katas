#include <concepts>
#include <iostream>
#include <cstdint>



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

template <typename T>
concept Integer = std::is_integral_v<T>;

template<typename T>
concept Integer32 = std::is_same_v<T, std::int32_t> || std::is_same_v<T, std::uint32_t>;


template<Integer32 T>
class MyClass{
    MyClass();
    MyClass(const T& i){

    }
};



int main(){


    std::cout << midpoint(2, 3.14) << '\n';

    std::cout << divide(-1, 2.5) << '\n';

}

