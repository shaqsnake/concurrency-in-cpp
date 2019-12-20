#include <thread>
#include <iostream>

class scoped_thread {
public:
    explicit scoped_thread(std::thread t_): t(std::move(t_)) {
        if (!t.joinable()) {
            throw std::logic_error("No thread");
        }
    }

    ~scoped_thread() {
        t.join();
    }

    scoped_thread(scoped_thread const&) = delete;
    scoped_thread& operator=(scoped_thread const&) = delete;
private:
    std::thread t;
};

struct func {
    int& i;

    func(int& i_): i(i_) {}

    void operator()()
    {
        for (unsigned j = 0; j < 100; j++) {
            std::cout << i << std::endl;
        }
    }
};

void f() {
    int some_local_state = 42;
    scoped_thread t((std::thread(func(some_local_state))));
} // 函数结束时，析构scoped_thread对象

int main() {
    f();
    return 0;
}