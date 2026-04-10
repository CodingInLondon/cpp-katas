

#include <iostream>
#include <vector>
#include <cstring>
#include <memory>





class Buffer {
    char* data;
    size_t size;
public:
    // Constructor
    Buffer(size_t s) : size(s) {
        data = new char[size];
        std::cout << "Constructed Buffer(" << size << ") at " << (void*)data << "\n";
    }
    
    // Destructor
    ~Buffer() {
        std::cout << "Destroying Buffer at " << (void*)data << "\n";
        delete[] data;
    }
    
    // Copy constructor (expensive!)
    Buffer(const Buffer& other) : size(other.size) {
        data = new char[size];
        std::memcpy(data, other.data, size);
        std::cout << "COPIED Buffer from " << (void*)other.data 
                  << " to " << (void*)data << "\n";
    }
    
    // TODO: We'll add move constructor here
    Buffer(Buffer&& other) noexcept : data(other.data), size(other.size){
        other.data = NULL;
        other.size = 0;

        std::cout << "MOVING " << (void*)data << "\n";

    }


    Buffer& operator=(Buffer&& other) noexcept{
        if (data != other.data){

            // free-up local buffer
            std::cout << "Destroying Buffer at " << (void*)data << "\n";
            delete[] data;

            // steal the other's buffer
            data = other.data;
            size = other.size;

            // clean the other's buffer
            other.data = nullptr;
            other.size = 0;
            std::cout << "MOVING " << (void*)data << "\n";

            return *this;
        }
        else{
            std::cout << "Self assignment detected for " << (void*)data << "\n";
            return *this;
        }
    }   


};

Buffer createBuffer() {
    return Buffer(1024);
}

std::unique_ptr<Buffer>createUniqueBuffer(){
    return std::make_unique<Buffer>(1024);
}


template <typename T>
void consume(T& x){

}

template <typename T>
void sink(T&& x) {
    consume(x);
}




int main() {
    // std::cout << "=== Creating vector ===\n";
    // std::vector<Buffer> buffers;
    
    // std::cout << "\n=== Adding buffer ===\n";
    // buffers.push_back(createBuffer());

    // std::cout << "=== Creating buffer ===\n";
    // auto buffer = createUniqueBuffer();

    // Buffer buffer(20);

    // sink(buffer);


    Buffer b1(1024);
    Buffer b2(2048);

    b1 = std::move(b2);  // Normal case: b1 gets b2's resources

    // Edge case:
    b1 = std::move(b1);  // Self-assignment (rare but must be safe!)

    std::cout << "\n=== Exiting ===\n";
    return 0;


}

