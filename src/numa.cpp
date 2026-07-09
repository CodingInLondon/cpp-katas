// Thread pinning (CPU affinity) and NUMA-local memory allocation on Linux.
//
// Build (affinity only):   g++ -std=c++17 -pthread numa.cpp -o numa
// Build (with libnuma):    g++ -std=c++17 -DUSE_LIBNUMA -pthread numa.cpp -o numa -lnuma
//   (libnuma needs the dev package, e.g. `sudo apt install libnuma-dev`)

#include <pthread.h>
#include <sched.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <thread>
#include <vector>

#if defined(USE_LIBNUMA)
#include <numa.h>
#endif

// Pin the calling thread to a single logical CPU. Returns 0 on success.
static int pin_to_core(int core) {
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(core, &cpuset);

    pthread_t self = pthread_self();
    int rc = pthread_setaffinity_np(self, sizeof(cpuset), &cpuset);
    if (rc != 0) {
        std::fprintf(stderr, "pthread_setaffinity_np failed: %s\n", std::strerror(rc));
    }
    return rc;
}

// Allocate `bytes` on the NUMA node that the calling thread is currently
// running on. Returns nullptr on failure. Free with free_local_buffer().
static void* alloc_local_buffer(std::size_t bytes) {
#if defined(USE_LIBNUMA)
    if (numa_available() < 0) {
        // Kernel/system has no NUMA support; fall back to a normal allocation.
        return std::malloc(bytes);
    }

    void* p = numa_alloc_local(bytes);
    if (p != nullptr) {
        std::memset(p, 0, bytes);  // first-touch on the local thread
    }
    return p;
#else
    // No libnuma: a plain allocation. The kernel's default "first-touch"
    // policy still places pages on the node of whichever core writes first,
    // and since we're pinned, that's the local node anyway for many workloads.
    void* p = std::malloc(bytes);
    if (p != nullptr) {
        std::memset(p, 0, bytes);
    }
    return p;
#endif
}

static void free_local_buffer(void* p, std::size_t bytes) {
#if defined(USE_LIBNUMA)
    if (numa_available() >= 0) {
        numa_free(p, bytes);  // must pass the same size given to numa_alloc_local
        return;
    }
#endif
    (void)bytes;
    std::free(p);
}

static void worker(int core) {
    if (pin_to_core(core) != 0) {
        return;
    }

    // Confirm where we actually ended up running.
    int running_on = sched_getcpu();

    // Pre-allocate a per-worker buffer on the local NUMA node.
    constexpr std::size_t kBufBytes = 64 * 1024 * 1024;  // 64 MiB
    void* buf = alloc_local_buffer(kBufBytes);
    if (buf == nullptr) {
        std::fprintf(stderr, "worker on core %d: allocation failed\n", core);
        return;
    }

#if defined(USE_LIBNUMA)
    int node = numa_node_of_cpu(running_on);
    std::printf("worker requested core %d, running on core %d, %zu MiB on NUMA node %d\n",
                core, running_on, kBufBytes >> 20, node);
#else
    std::printf("worker requested core %d, running on core %d, %zu MiB allocated (no libnuma)\n",
                core, running_on, kBufBytes >> 20);
#endif

    // ... do work here on `buf`; pages are local to this pinned thread ...

    free_local_buffer(buf, kBufBytes);
}

int main() {
    unsigned hw = std::thread::hardware_concurrency();
    std::printf("hardware_concurrency = %u\n", hw);

    // Pin two workers to distinct cores (guard against machines with few cores).
    int core_a = 0;
    int core_b = (hw > 1) ? 1 : 0;

    std::thread t1(worker, core_a);
    std::thread t2(worker, core_b);

    t1.join();
    t2.join();
    return 0;
}
