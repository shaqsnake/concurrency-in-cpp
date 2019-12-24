#include <exception>
#include <mutex>
#include <stack>
#include <memory>
#include <iostream>
#include <thread>

struct empty_stack: std::exception {
    const char* what() const throw() {
        return "empty stack!"; 
    };
};

template<typename T>
class threadsafe_stack {
private:
    std::stack<T> data;
    mutable std::mutex m;

public:
    threadsafe_stack(): data(std::stack<T>()) {}
    threadsafe_stack(const threadsafe_stack& other) {
        std::lock_guard<std::mutex> lock(other.m); // 拷贝前先调用锁，保证拷贝操作正确
        data = other.data;
    }
    threadsafe_stack& operator=(const threadsafe_stack&) = delete;

public:
    void push(T new_value);
    std::shared_ptr<T> pop();
    void pop(T& value);
    bool empty() const;
};

template<typename T>
bool threadsafe_stack<T>::empty() const{
    std::lock_guard<std::mutex> lock(m);
    return data.empty();
}

template<typename T>
void threadsafe_stack<T>::push(T new_value) {
    std::lock_guard<std::mutex> lock(m);
    data.push(new_value);
}

template<typename T>
std::shared_ptr<T> threadsafe_stack<T>::pop() {
    std::lock_guard<std::mutex> lock(m);
    if (data.empty()) throw empty_stack();

    auto const res = std::make_shared<T>(data.top());
    data.pop();
    return res;
}

template<typename T>
void threadsafe_stack<T>::pop(T& value) {
    std::lock_guard<std::mutex> lock(m);
    if (data.empty()) throw empty_stack();

    value = data.top();
    data.pop();
}

int main() {
    auto ts_stack = threadsafe_stack<int>();
    std::thread t1(&threadsafe_stack<int>::push, &ts_stack, 10);
    std::thread t2(&threadsafe_stack<int>::push, &ts_stack, 20);
    t1.join();
    t2.join();

    std::cout << ts_stack.empty() << std::endl;
    std::cout << *ts_stack.pop() << std::endl;
    std::cout << *ts_stack.pop() << std::endl;

    return 0;
}