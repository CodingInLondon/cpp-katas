#pragma once
#include <cstddef>
#include <memory>
#include <vector>
#include <array>
#include <algorithm>
#include <cassert>

template <typename T, std::size_t capacity>
class Pool{


    struct alignas(T) Storage{
        std::array<std::byte, capacity * sizeof(T)> buffer;
    };

    public:

    Pool() {
        // use make_unique to ensure it lives on the heap (could be too large for the stack)
        _storage = std::make_unique<Storage>();

        // Initialise the free list
        for(std::size_t i = 0; i < capacity; ++i){
            auto p = _storage.get()->buffer.data() + i * (sizeof(T));
            _free_list.push_back(reinterpret_cast<T*>(p));
        }

    }
    
    template <typename... Args> 
    T* allocate(Args&&... args){

        // Pop from the free list

        if (_free_list.empty())
            return nullptr;
        else{

            auto p = _free_list.back();

            T* object = std::construct_at(p, std::forward<Args>(args)...);

            _free_list.pop_back();

            return object;
        }
    }

    void release(T* t){

        assert(t != nullptr);
        assert(_free_list.size() < capacity);
        assert(!std::ranges::contains(_free_list, t));

        if (t == nullptr || _free_list.size() == capacity)
            return;
        else if (!std::ranges::contains(_free_list, t)) [[likely]]
        {
            // Push to the free list
            std::destroy_at(t);
            _free_list.push_back(t);

        }
    }

    private:

    std::vector<T*> _free_list;


    std::unique_ptr<Storage> _storage;

};

