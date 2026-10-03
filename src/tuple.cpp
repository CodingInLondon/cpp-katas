
#include <tuple>
#include <string>
#include <cstdint>
#include <print>

int main(){

    std::tuple<std::string, std::uint64_t, double> num;

    std::get<0>(num) = "hello";
    std::get<1>(num) = 64;
    std::get<2>(num) = 3.1444444;


    num = {"hello", 64, 3.14444};

    auto& [instrument, price, timestamp] = num;

    std::println("instrument: {}",instrument);
    std::println("price: {}",price);

}