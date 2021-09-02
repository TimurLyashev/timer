#ifndef TIMER_H
#define TIMER_H

#include <atomic>
#include <functional>
#include <condition_variable>
#include <thread>

#ifndef NDEBUG // NDEBUG is cmake-specific variable with not-debug logic parameter
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

    std::atomic<bool> stop_flag = ATOMIC_VAR_INIT(false);

    std::condition_variable start_cv;
    std::mutex start_mutex;

    void loop()
    {
        std::unique_lock<std::mutex> start_locker(start_mutex);
        start_cv.wait(start_locker);

        while (!stop_flag.load()) {
            std::apply(callable_function, callable_function_args);
            std::this_thread::sleep_for(tick);
        }
    }

public:
    explicit Timer(int new_tick, Callable&& function, Args&&... args)
        : tick(std::move(new_tick))
        , callable_function(function)
        , callable_function_args(args...)
    {
#ifndef NDEBUG // NDEBUG is cmake-specific variable with not-debug logic parameter
        std::cout << "timer func: " << (int*)function << "\n";
        expand_type { 0, (std::cout << args << "\n", 0)... };
#endif
    }

    void create()
    {
        loop_thread = std::make_unique<std::thread>(&Timer::loop, this);
        loop_thread->detach();
    }

    void start()
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        std::unique_lock<std::mutex> start_locker(start_mutex);
        start_locker.unlock();
        start_cv.notify_one();
    }

    void stop()
    {
        stop_flag.store(true);
    }

};

#endif // TIMER_H
