#pragma once
#include <span>
#include <cstring>
#include <bit>

// Used to receive the MoldUDP64 header
struct MoldHeader{
    std::span<const std::byte, 10> session; //0
    std::uint64_t seq;                      //10
    std::uint16_t count;                    //18
};

constexpr std::size_t kMoldHeaderSize = 20;
constexpr std::size_t kEndOfSession = 0xFFFF;

// Helper function to convert from wire format to host
template <std::integral T>
T load_be(std::span<const std::byte, sizeof(T)> b){

    T destination;
    std::memcpy(&destination, b.data(), sizeof(T));

    if constexpr (std::endian::native == std::endian::little){
        destination = std::byteswap(destination);
    }

    return destination;
}

// Parse the MoldUPD64 header (which contains ITCH messages)
template <class Sink>
bool parse_mold(std::span<const std::byte> pkt, Sink& sink){
    if (pkt.size() < kMoldHeaderSize)
        return false;

    std::byte* p = pkt.data();

    MoldHeader header{std::span<const std::byte, 10>{pkt, 10},
                        load_be<std::uint64_t>(pkt.subspan<10,8>()),
                        load_be<std::uint16_t>(pkt.subspan<18,2>())};

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

        std::uint16_t message_length = load_be<std::uint16_t>(pkt.subspan(offset).first<2>());

        if (message_length > 0){
            parse_itch(pkt.subspan(offset+2).first(message_length), sink);
            
        }

        offset += 2 + message_length;
    }    

    return true;
}

// Parse one ITCH message
template <class Sink>
bool parse_itch(std::span<const std::byte> pkt, Sink& sink){

    return true;
}

