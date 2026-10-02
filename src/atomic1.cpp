#include <atomic>
#include <thread>
#include <iostream>

std::atomic<bool> ready = false;
std::atomic<int> data = 0;

void producer() {
    data = 42;
    ready.store(true, std::memory_order_release);
}

void consumer() {
    while (!ready.load(std::memory_order_acquire)) {}
    std::cout << data << "\n";
}

int main(){

    int counter = 0;

    std::atomic_ref<int> count{counter};

    return 1;
}