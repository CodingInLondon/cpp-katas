#include <variant>
#include <string>
#include <iostream>

int main()
{
    std::variant<int, double, std::string> data = "Hello";

    std::visit([](auto&& value) {          // lambda that works for any type
        std::cout << "The value is: " << value << '\n';
    }, data);

    return 0;
}