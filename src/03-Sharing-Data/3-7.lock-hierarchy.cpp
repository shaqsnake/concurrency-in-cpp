#include <mutex>
#include <climits>
#include <iostream>
#include <thread>

class hierarchical_mutex {
    std::mutex internal_mutex;

    unsigned long const hierarchy_value;
    unsigned long previous_hierarchy_value;

    static thread_local unsigned long this_thread_hierachy_value; // 当前线程的层级值，声明未thread_local保证每个线程中此值独立

    void check_for_hierachy_violation() {
        if (this_thread_hierachy_value <= hierarchy_value) // 判断当前的层级值是否大于新上锁的层级值
            throw std::logic_error("mutex hierarchy violated");
    }

    void update_hierachy_value() {
        previous_hierarchy_value = this_thread_hierachy_value; // 每次更新层级锁时需要把当前值保存下来
        this_thread_hierachy_value = hierarchy_value;
    }

public:
    explicit hierarchical_mutex(unsigned long value): hierarchy_value(value), previous_hierarchy_value(0) {}

    void lock() {
        check_for_hierachy_violation();
        internal_mutex.lock(); // 更新前先上锁
        update_hierachy_value(); // 更新层级锁，用新的层级替换
    }

    void unlock() {
        if (this_thread_hierachy_value != hierarchy_value) // 判断解锁的过程是否是符合顺序的，即当前层级锁只能由自己解开
            throw std::logic_error("mutex hierarchy violated");
        this_thread_hierachy_value = previous_hierarchy_value; // 解锁时需要恢复上阶段的层级，在更新时已经保存在previous_hierarchy_value中
        internal_mutex.unlock();
    }

    bool try_lock() {
        check_for_hierachy_violation();
        if (!internal_mutex.try_lock()) // 如果internal_mutex上锁失败，返回false
            return false;
        update_hierachy_value();
        return true;
    }
};

thread_local unsigned long hierarchical_mutex::this_thread_hierachy_value(ULONG_MAX); // 当前线程层级值初始化为最大，保证开始阶段都能成功上锁

hierarchical_mutex high_level_mutex(10000);
hierarchical_mutex low_level_mutex(5000);
hierarchical_mutex other_mutex(3000);

void low_level_func() {
    std::lock_guard<hierarchical_mutex> lk(low_level_mutex);
    std::cout << "Executing low_level_func ..." << std::endl;
}

void thread_a() {
    std::lock_guard<hierarchical_mutex> lk(high_level_mutex);
    std::cout << "Runing thread_a ..." << std::endl;
    low_level_func();
}

void thread_b() {
    std::lock_guard<hierarchical_mutex> lk(other_mutex);
    std::cout << "Runing thread_b ..." << std::endl;
    low_level_func();
}

int main() {
    std::thread t1(thread_a);
    std::thread t2(thread_b);

    t1.join();
    t2.join();

    return 0;
}
