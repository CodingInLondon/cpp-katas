#include <coroutine>
#include <iostream>

struct Task {
    struct promise_type {
        Task get_return_object() {
            return Task{std::coroutine_handle<promise_type>::from_promise(*this)};
        }
        std::suspend_never initial_suspend() noexcept { return {}; }
        std::suspend_never final_suspend() noexcept { return {}; }
        void return_void() {}
        void unhandled_exception() { std::terminate(); }
    };

    std::coroutine_handle<promise_type> handle;
};

Task async_work() {
    std::cout << "Start work\n";
    co_await std::suspend_always{}; // suspend coroutine here
    std::cout << "Resume work\n";
}

int main() {
    Task t = async_work();      // runs up to the co_await, prints "Start work"
    std::cout << "Suspended, back in main\n";
    t.handle.resume();          // prints "Resume work", coroutine runs to completion
}
