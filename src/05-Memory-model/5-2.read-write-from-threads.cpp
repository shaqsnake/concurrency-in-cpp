#include <vector>
#include <atomic>
#include <iostream>
#include <thread>
#include <chrono>

std::vector<int> data;
std::atomic<bool> data_ready(false);

void read_thread() {
    while (!data_ready.load()) { // 保证访问顺序是先写后读
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        std::cout << "The answer is " << data[0] << std::endl;
    }
}

void write_thread() {
    data.push_back(42);
    std::cout << "Send data" << std::endl;
    data_ready = true;
}

int main() {
    std::thread t1(read_thread);
    std::thread t2(write_thread);

    t2.detach();
    t1.join();

    return 0;
}