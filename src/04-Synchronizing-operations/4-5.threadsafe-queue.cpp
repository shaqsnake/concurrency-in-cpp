#include <queue>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <iostream>
#include <thread>

template<typename T>
class threadsafe_queue {
    mutable std::mutex mu; // 互斥量需要时mutable，这样才能在const函数和对象中上锁
    std::queue<T> data_queue;
    std::condition_variable data_cond;

public:
    threadsafe_queue() = default;

    threadsafe_queue(threadsafe_queue const& other) {
        std::lock_guard<std::mutex> lock(other.mu);
        data_queue = other.data_queue;
    }

    void push(T new_value) {
        std::lock_guard<std::mutex> lock(mu);
        data_queue.push(new_value);
        data_cond.notify_one();
    }

    void wait_and_pop(T& value) {
        std::unique_lock<std::mutex> lock(mu);
        data_cond.wait(lock, [this]{ return !data_queue.empty(); });
        value = data_queue.front();
        data_queue.pop();
    }

    std::shared_ptr<T> wait_and_pop() {
        std::unique_lock<std::mutex> lock(mu);
        data_cond.wait(lock, [this]{ return !data_queue.empty(); });
        std::shared_ptr<T> res(std::make_shared<T>(data_queue.front()));
        data_queue.pop();
        return res;
    }

    bool try_pop(T& value) {
        std::lock_guard<std::mutex> lock(mu);
        if (data_queue.empty())
            return false;
        value = data_queue.front();
        data_queue.pop();
        return true;
    }

    std::shared_ptr<T> try_pop() {
        std::lock_guard<std::mutex> lock(mu);
        if (data_queue.empty())
            return std::shared_ptr<T>();
        std::shared_ptr<T> res(std::make_shared<T>(data_queue.front()));
        data_queue.pop();
        return res;
    }

    bool empty() const {
        std::lock_guard<std::mutex> lock(mu);
        return data_queue.empty();
    }
};

threadsafe_queue<int> q;

void data_preparation() {
    for (int i = 1; i < 10; i++) {
        q.push(i);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        std::cout << "Preparing data " << i << std::endl;
    }
}

void data_processing() {
    while (true) {
        int data;
        q.wait_and_pop(data);
        std::cout << "Processing data " << data << std::endl;
        if (data >= 9) break;
    }
}

int main() {
    std::thread t1(data_preparation);
    std::thread t2(data_processing);
    t1.detach();
    t2.join();

    return 0;
}