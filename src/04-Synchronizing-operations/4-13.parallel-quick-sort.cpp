#include <list>
#include <algorithm>
#include <iostream>
#include <future>

template<typename T>
std::list<T> parallel_quick_sort(std::list<T> input) {
    if (input.empty())
        return input;

    std::list<T> result;
    result.splice(result.begin(), input, input.begin());
    T const& pivot = *result.begin();

    auto divide_point = std::partition(input.begin(), input.end(), [&](T const& t) { return t < pivot; });

    std::list<T> lower_part;
    lower_part.splice(lower_part.end(), input, input.begin(), divide_point);

    std::future<std::list<T>> new_lower(std::async(&parallel_quick_sort<T>, std::move(lower_part))); // 启动新线程进行排序
    auto new_higher(parallel_quick_sort(std::move(input)));

    result.splice(result.end(), new_higher);
    result.splice(result.begin(), new_lower.get()); // 等待后台完成后将结果合并
    return result;
}

int main() {
    std::list<int> l{3, 6, 1, 6, 2, 4, 8, 4};
    for (auto const& x: parallel_quick_sort(l))
        std::cout << x << " ";
    std::cout << std::endl;

    return 0;
}