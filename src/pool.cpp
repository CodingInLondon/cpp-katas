#include <cstddef>
#include <array>
#include <memory>


template <typename T, std::size_t N>
class ObjectPool{
    public:

    ObjectPool(){

        for(std::size_t i=0; i< N ; i++){
            freeList[i] = &buffer[i*sizeof(T)];
        }

        freeTop = N;
    }

    ObjectPool(ObjectPool& p) = delete;
    ObjectPool(ObjectPool&& p) = delete;


    T* Acquire(){

        if (freeTop == 0)
            return nullptr;

        --freeTop;// remove from free list

        T* obj = std::construct_at(reinterpret_cast<T*>(freeList[freeTop]));

        return obj;
    }


    void Release(T* object){

        if (freeTop >= N)
            return;

        // Destroy
        std::destroy_at(object);

        // Add to free list
        freeList[freeTop] = reinterpret_cast<std::byte*>(object);

        if (freeTop < (N-1))
            ++freeTop; 

    }


    private:


    alignas(T) std::array<std::byte, N*sizeof(T)> buffer;

    //free list
    std::array<std::byte*, N> freeList;
    std::size_t freeTop{0}; // Number of available slots

};

struct Order{
    std::uint64_t price;
    char instrument[5];
};

int main(){

    ObjectPool<Order, 100> pool;


    return 1;
};

