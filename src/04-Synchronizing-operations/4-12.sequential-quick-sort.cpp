#include <list>
#include <iostream>
#include <algorithm>

template<typename T>
std::list<T> sequential_quick_sort(std::list<T> input) {
    if (input.empty())
        return input;
    
    std::list<T> result;
    result.splice(result.begin(), input, input.begin()); // 将输入的第一个元素放在result第一个位置
    T const& pivot = *result.begin(); // 把result第一个元素作为pivot

    auto divide_point = std::partition(input.begin(), input.end(), [&](T const& t) { return t < pivot; }); // 将输入元素根据pivot的大小分成两个部分

    std::list<T> lower_part;
    lower_part.splice(lower_part.end(), input, input.begin(), divide_point); // 把值较小的一部分移入lower_part
    auto new_lower(sequential_quick_sort(std::move(lower_part))); // 递归
    auto new_higher(sequential_quick_sort(std::move(input))); // 递归

    result.splice(result.end(), new_higher);
    result.splice(result.begin(), new_lower);
    return result;
}

int main() {
    std::list<int> l{3, 6, 1, 6, 2, 4, 8, 4};
    for (auto const& x: sequential_quick_sort(l))
        std::cout << x << " ";
    std::cout << std::endl;

    return 0;
}