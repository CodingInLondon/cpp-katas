#pragma once
#include <cstddef>
#include <memory>
#include <vector>
#include <array>
#include <algorithm>
#include <cassert>
#include <cstdint>

// Every object allocated with allocate needs a matching call to release
// Pool does not destroy objects when destroyed.
template <typename T, std::size_t capacity> //capacity is the number of T objects that can be stored in the pool
requires std::is_trivially_copyable_v<T>
class Pool{

    struct alignas(T) Storage{
        std::array<std::byte, capacity * sizeof(T)> buffer{};
    };

    public:

    Pool() {
        // use make_unique to ensure it lives on the heap (could be too large for the stack)
        _storage = std::make_unique<Storage>();

        _free_list.reserve(capacity);

        // Initialise the free list
        for(std::size_t i = 0; i < capacity; ++i){
            auto p = _storage->buffer.data() + i * (sizeof(T));
            _free_list.push_back(reinterpret_cast<T*>(p));
        }

    }
    
    template <typename... Args> 
    T* allocate(Args&&... args){

        // Pop from the free list

        if (_free_list.empty()) [[unlikely]]
            return nullptr;

        auto p = _free_list.back();

        T* object = std::construct_at(p, std::forward<Args>(args)...);

        _free_list.pop_back();

        return object;
    }

    void release(T* t){

        assert(t != nullptr);
        assert(_free_list.size() < capacity);
        assert(!std::ranges::contains(_free_list, t));
        assert(is_in_buffer(t));

        if (t == nullptr || _free_list.size() == capacity) [[unlikely]]
            return;

        if (!is_in_buffer(t)) [[unlikely]]
            return;

        // Push to the free list
        std::destroy_at(t);
        _free_list.push_back(t);
    }

    private:

    // Check the provided pointer is within our buffer mem range
    bool is_in_buffer(const T* t) const{
        auto base = reinterpret_cast<std::uintptr_t>(_storage->buffer.data());
        auto edge = base + sizeof(Storage);
        auto to_release = reinterpret_cast<std::uintptr_t>(t);

        if ( ! ( (to_release >= base) && (to_release < edge))) [[unlikely]]
            return false; 

        // Check the provided pointer is aligned on T
        if ( (to_release - base) % sizeof(T) != 0 )
            return false;

        return true;
    }

    std::vector<T*> _free_list;

    std::unique_ptr<Storage> _storage;

};

