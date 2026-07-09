#include <coroutine>
#include <iostream>
#include <exception>

struct IntGenerator {
    struct promise_type {
        int current_value{};

        IntGenerator get_return_object() {
            return IntGenerator{
                std::coroutine_handle<promise_type>::from_promise(*this)
            };
        }

        std::suspend_always initial_suspend() {
            return {};
        }

        std::suspend_always final_suspend() noexcept {
            return {};
        }

        std::suspend_always yield_value(int value) {
            current_value = value;
            return {};
        }

        void return_void() {}

        void unhandled_exception() {
            std::terminate();
        }
    };

    using Handle = std::coroutine_handle<promise_type>;

    explicit IntGenerator(Handle h)
        : handle(h) {}

    IntGenerator(const IntGenerator&) = delete;

    IntGenerator(IntGenerator&& other) noexcept
        : handle(other.handle) {
        other.handle = nullptr;
    }

    ~IntGenerator() {
        if (handle) {
            handle.destroy();
        }
    }

    bool next() {
        handle.resume();
        return !handle.done();
    }

    int value() const {
        return handle.promise().current_value;
    }

private:
    Handle handle;
};

IntGenerator count_to(int n) {
    for (int i = 1; i <= n; ++i) {
        co_yield i;
    }
}

int main() {
    std::cout << "start\n";
    
    auto gen = count_to(3);

    while (gen.next()) {
        std::cout << gen.value() << '\n';
    }
}

