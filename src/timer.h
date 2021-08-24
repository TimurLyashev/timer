#ifndef TIMER_H
#define TIMER_H

#include <atomic>
#include <functional>
#include <thread>

#ifdef DEBUG
#include <iostream>

struct expand_type {
    template <typename... T>
    expand_type(T&&...) { }
};
#endif

template <typename Callable, typename... Args>
class Timer {
    void loop()
    {
        while (!start_flag.test_and_set(std::memory_order_acquire)) {
            start_flag.clear(std::memory_order_release);
        }

        start_flag.test_and_set(std::memory_order_acquire);

        while (!stop_flag.test_and_set(std::memory_order_acquire)) {
            std::apply(callable_function, callable_function_args);
            std::this_thread::sleep_for(tick);
            stop_flag.clear(std::memory_order_release);
        }
    }

    std::unique_ptr<std::thread> loop_thread;

    const std::chrono::milliseconds tick;

    std::function<Callable> callable_function;
    std::tuple<Args...> callable_function_args;

    std::atomic_flag start_flag = ATOMIC_FLAG_INIT;
    std::atomic_flag stop_flag = ATOMIC_FLAG_INIT;

public:
    void create()
    {
        loop_thread = std::make_unique<std::thread>(&Timer::loop, this);
        loop_thread->detach();
    }

    void start()
    {
        start_flag.test_and_set(std::memory_order_acquire);
    }

    void stop()
    {
        stop_flag.test_and_set(std::memory_order_acquire);
    }

    explicit Timer(int new_tick, Callable&& __f, Args&&... __args)
        : callable_function(__f)
        , callable_function_args(__args...)
        , tick(new_tick)
    {
#ifdef DEBUG
        std::cout << "timer func: " << (int*)__f << "\n";
        expand_type { 0, (std::cout << __args << "\n", 0)... };
#endif
    }
};

#endif // TIMER_H
