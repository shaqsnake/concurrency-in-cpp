#include <numeric>
#include <vector>
#include <thread>
#include <iostream>

template<typename Itrerator, typename T>
struct accumulate_block {
    void operator()(Itrerator first, Itrerator last, T& result) {
        result = std::accumulate(first, last, result);
    }
};

template<typename Iterator, typename T>
T parrallel_accumulate(Iterator first, Iterator last, T init) {
    unsigned long const length = std::distance(first, last);

    if (!length) // 输入范围为空
        return init;

    unsigned long const min_per_thread = 25;
    unsigned long const max_threads = (length + min_per_thread - 1) / min_per_thread; // 启动线程的最大数量

    unsigned long const hardware_threads = std::thread::hardware_concurrency(); // 硬件支持的线程数量

    unsigned long const num_threads = std::min(hardware_threads != 0 ? hardware_threads : 2, max_threads);

    unsigned long const block_size = length / num_threads; // 分配给每个线程中处理元素的数量

    std::vector<T> results(num_threads); // 存放中间结果
    std::vector<std::thread> threads(num_threads - 1); // 简单线程池，少启动一个线程是因为有主线程

    Iterator block_start = first;
    for (unsigned long i = 0; i < (num_threads - 1); i++) {
        Iterator block_end = block_start;
        std::advance(block_end, block_size);
        threads[i] = std::thread(
            accumulate_block<Iterator, T>(),
            block_start,
            block_end,
            std::ref(results[i]));
        block_start = block_end;
    }
    accumulate_block<Iterator, T>()(
        block_start, last, results[num_threads - 1]); // 主线程处理未分配的剩余部分
    
    for (auto& entry: threads)
        entry.join(); // 等待所有线程完成工作

    return std::accumulate(results.begin(), results.end(), init); // 将所有中间结果累加后输出
}

int main() {
    std::vector<int> vec = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    std::cout << parrallel_accumulate(vec.begin(), vec.end(), 0) << std::endl;

    return 0;
}