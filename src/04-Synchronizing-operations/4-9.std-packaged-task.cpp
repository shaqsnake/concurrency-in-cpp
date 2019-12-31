#include <deque>
#include <mutex>
#include <future>
#include <iostream>

std::mutex mu;
std::deque<std::packaged_task<void()>> tasks;
unsigned int id = 0;

bool gui_shutdown_message_received() {
    return id == 10;
}

void get_and_process_gui_message() {
    std::cout << "Processing data with id = " << id << std::endl;
}

void gui_thread() { // 后台线程
    while(!gui_shutdown_message_received()) { // 轮询处理消息，直到达到关闭的条件
        get_and_process_gui_message();
        std::packaged_task<void()> task;
        {
            std::lock_guard<std::mutex> lock(mu);
            if (tasks.empty()) continue; // 如果队列中没有任务就继续轮询
            task = std::move(tasks.front()); // 从队列中提取任务
            tasks.pop_front();
        }
        task(); // 释放锁然后执行任务，避免上锁时间过长
    }
}

template<typename Func>
std::future<void> post_task_for_gui_thread(Func f) {
    std::packaged_task<void()> task(f); // 将任务打包
    std::future<void> res = task.get_future(); // 可以获取期望值
    std::lock_guard<std::mutex> lock(mu);
    tasks.push_back(std::move(task)); // 将任务压入队列
    return res; // 返回期望值
}

void exec_task() {
    std::cout << "Executing task with id = " << id << std::endl;
    id ++;
}

int main() {
    std::thread gui_bg_thread(gui_thread);

    for (int i = 0; i < 10; i++) {
        post_task_for_gui_thread(exec_task); 
    }

    gui_bg_thread.join();

    return 0;
}