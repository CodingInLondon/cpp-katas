#pragma once
#include <cstddef>
#include <memory>
#include <vector>

template <typename T, std::size_t capacity>
class Pool{


    public:

    Pool() {
        // use make_unique to ensure it leaves on the heap (could be too large for the stack)
        _buffer = std::make_unique<std::array<std::byte, capacity* sizeof(T)>>();

        // Initialise the free list
        for(std::size_t i = 0; i < capacity; ++i){
            auto p = _buffer.get()->data() + i * (sizeof(T));
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

        // Push to the free list

        _free_list.push_back(t);

        std::destroy_at(t);

    }

    private:

    std::vector<T*> _free_list;


    std::unique_ptr<std::array<std::byte, capacity* sizeof(T)>> _buffer;

};

