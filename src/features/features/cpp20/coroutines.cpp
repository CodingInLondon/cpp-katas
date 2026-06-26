// C++20: Coroutines
// A lazy generator that yields values one at a time via co_yield.
#include <coroutine>
#include <cstdint>
#include <iostream>

struct Generator {
    struct promise_type {
        std::int64_t current;
        Generator get_return_object() {
            return Generator{std::coroutine_handle<promise_type>::from_promise(*this)};
        }
        std::suspend_always initial_suspend() noexcept { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        std::suspend_always yield_value(std::int64_t v) noexcept {
            current = v;
            return {};
        }
        void return_void() {}
        void unhandled_exception() { std::terminate(); }
    };

    std::coroutine_handle<promise_type> handle;
    explicit Generator(std::coroutine_handle<promise_type> h) : handle(h) {}
    ~Generator() { if (handle) handle.destroy(); }
    Generator(const Generator&) = delete;
    Generator& operator=(const Generator&) = delete;

    bool next() {
        handle.resume();
        return !handle.done();
    }
    std::int64_t value() const { return handle.promise().current; }
};

Generator fibonacci(int count) {
    std::int64_t a = 0, b = 1;
    for (int i = 0; i < count; ++i) {
        co_yield a;
        std::int64_t next = a + b;
        a = b;
        b = next;
    }
}

int main() {
    auto gen = fibonacci(10);
    while (gen.next()) std::cout << gen.value() << ' ';
    std::cout << '\n';
    return 0;
}
