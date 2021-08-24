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
    std::unique_ptr<std::thread> loop_thread;

    const std::chrono::milliseconds tick;

    std::function<Callable> callable_function;
    std::tuple<Args...> callable_function_args;

    std::atomic<bool> start_flag = ATOMIC_VAR_INIT(false);
    std::atomic<bool> stop_flag = ATOMIC_VAR_INIT(false);

    void loop()
    {
        while (!start_flag.load()) {
        }

        while (!stop_flag.load()) {
            std::apply(callable_function, callable_function_args);
            std::this_thread::sleep_for(tick);
        }
    }

public:
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

    void create()
    {
        loop_thread = std::make_unique<std::thread>(&Timer::loop, this);
        loop_thread->detach();
    }

    void start()
    {
        start_flag.store(true);
    }

    void stop()
    {
        stop_flag.store(true);
    }

};

#endif // TIMER_H
