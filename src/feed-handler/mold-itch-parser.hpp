#include <span>
#include <cstdint>


struct MoldHeader{
    std::span<const std::byte, 10> session; //0
    std::uint64_t seq;                      //10
    std::uint16_t count;                    //18
};

constexpr std::size_t kMoldHeaderSize = 20;
constexpr std::size_t kEndOfSession = 0xFFFF;

template <class Sink>
bool parse_mold(std::span<const std::byte> pkt, Sink& sink){
    if (pkt.size() < kMoldHeaderSize)
        return false;

    std::byte* p = pkt.data();

    MoldHeader header{std::span<const std::byte, 10>{pkt, 10},
                        load_be<std::uint64_t>(pkt+10),
                        load_be<std::uint16_t>(pkt+18)};

    if (header.count == 0){
        sink.on_hearbeat(header);
        return true;
    }              

    if (header.count == kEndOfSession){
        sink.end_of_session(header);
        return true;
    }  


    std::size_t offset = kMoldHeaderSize;

    for(auto i = 0; i<header.count ; ++i){

        std::uint16_t length = load_be<std::uint16_t>(offset);

        if (length > 0){
            
        }



        offset = 

    }



    
}