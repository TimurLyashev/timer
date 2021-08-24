#include "timer.h"

#include <iostream>

void test_message_counter(int& counter)
{
    counter++;
    std::cout << counter << "\n";
}

int main()
{
    int counter = 0;
    int timer_tick = 100; //milliseconds
    int pause = 10; //seconds

    std::cout << "programm func: " << (int*)test_message_counter << "\n";
    Timer<void(int&), int&> test_timer(timer_tick, test_message_counter, counter);

    test_timer.create();
    test_timer.start();

    std::this_thread::sleep_for(std::chrono::seconds(pause));

    test_timer.stop();

    return 0;
}
