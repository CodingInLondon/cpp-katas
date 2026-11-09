#include <string>
#include <ranges>
#include <print>

std::string_view parse(std::string_view fix){

    // while(fix.find("|")){

    // }

    // for(auto token : fix | std::string_view::split)

    auto tokens =  fix | std::views::split('|'); 

    for(auto token : tokens){

        std::string_view field(token.begin(), token.end());

        std::println("{}", field);
    }
    
    return "";
}


int main(){

    parse("8=FIX.4.4|35=D|55=VOD.L|");



    return 0;
}