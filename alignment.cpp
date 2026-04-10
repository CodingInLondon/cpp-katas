#include <thread>
#include <iostream>
#include <chrono>
#include <limits>

struct Counters {
    int a{0};
    int b{0};
};

struct CountersAligned {
    alignas(64) int a{0};
    alignas(64) int b{0};
};

template<typename T>
void incrementA(T& c, int n) {
    for (int i = 0; i < n; ++i)
        ++c.a;
}

template<typename T>
void incrementB(T& c, int n) {
    for (int i = 0; i < n; ++i)
        ++c.b;
}

template<typename T>
long long run(T& c, int n) {
    auto start = std::chrono::steady_clock::now();

    std::thread t1(incrementA<T>, std::ref(c), n);
    std::thread t2(incrementB<T>, std::ref(c), n);

    t1.join();
    t2.join();

    auto end = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
}

int main() {
    const int n = std::numeric_limits<int>::max();

    Counters c1;
    std::cout << "False sharing:    " << run(c1, n) << " ms\n";

    CountersAligned c2;
    std::cout << "No false sharing: " << run(c2, n) << " ms\n";
}
