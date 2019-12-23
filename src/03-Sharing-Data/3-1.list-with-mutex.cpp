#include <list>
#include <mutex>
#include <algorithm>
#include <thread>
#include <iostream>

std::list<int> some_list;
std::mutex some_mutex;

void add_to_list(int new_value) {
    std::lock_guard<std::mutex> guard(some_mutex);
    // std::lock_guard guard(some_mutex); // c++17
    some_list.push_back(new_value);
}

bool list_contains(int value_to_find) {
    std::lock_guard<std::mutex> guard(some_mutex);
    // std::lock_guard guard(some_mutex); // c++17
    return std::find(some_list.begin(), some_list.end(), value_to_find) != some_list.end();
}

int main() {
    std::thread t1(add_to_list, 1);
    std::thread t2(add_to_list, 2);

    t1.detach();
    t2.detach();

    std::cout << list_contains(2) << std::endl;

    return 0;
}