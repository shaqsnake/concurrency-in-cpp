#include <mutex>
#include <queue>
#include <condition_variable>
#include <thread>
#include <iostream>
#include <chrono>

std::mutex m;
std::queue<int> q; // 用来传递数据的队列
std::condition_variable cond;

void data_preparation() {
    for (int i = 1; i <= 10; i++) {
        int const data = i;
        std::lock_guard<std::mutex> lock(m); // 先上锁再push
        q.push(data);
        cond.notify_one();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        std::cout << "Sending data " << i << std::endl;
    }
}

void data_processing() {
    while (true) {
        std::unique_lock<std::mutex> lock(m);
        cond.wait(lock, []{ return !q.empty(); }); // 传递锁和等待条件
        auto data = q.front();
        q.pop();
        lock.unlock(); // 后面的处理过程耗时，可以先释放锁
        // std::this_thread::sleep_for(std::chrono::milliseconds(100));
        std::cout << "Receiving data " << data << std::endl;

        if (data >= 10) break;
    }
}

int main() {
    std::thread t1(data_processing);
    std::thread t2(data_preparation);
    t1.join();
    t2.join();

    return 0;
}