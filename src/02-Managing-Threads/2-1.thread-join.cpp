#include <iostream>
#include <thread>

class thread_guard {
public:
    explicit thread_guard(std::thread& t_) :
        t(t_)
    {}
    ~thread_guard()
    {
        if (t.joinable()) {
            t.join(); // 析构时将当前线程join到原线程
        }
    }
    thread_guard(thread_guard const&) = delete; // 移除拷贝构造函数
    thread_guard& operator=(thread_guard const&) = delete; // 移除拷贝赋值函数

private:
    std::thread& t;
};

struct func {
    int& i;

    func(int& i_) :
        i(i_)
    {}

    void operator()()
    {
        for (unsigned j = 0; j < 10000; j++) {
            std::cout << i << std::endl;
        }
    }
};

void oops()
{
    int some_local_state = 0;
    func my_func(some_local_state);
    std::thread my_thread(my_func);
    my_thread.detach();  // 不等待线程结束，可能会造成线程中的引用悬空
}

void f() {
    int some_local_state = 0;
    func my_func(some_local_state);
    std::thread my_thread(my_func);
    thread_guard g(my_thread); // 函数结束时销毁g，此时调用thread_guard的析构函数
}

int main()
{
    // oops();
    f();
    return 0;
}