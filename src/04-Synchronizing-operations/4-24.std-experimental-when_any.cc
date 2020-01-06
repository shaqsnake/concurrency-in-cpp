#include <experimental/future>

std::experimental::future<FinalResult> find_and_process_value(std::vector<MyData> &data) {
    unsigned const concurrency = std::thread::hardware_concurrency();
    unsigned const num_tasks = (concurrency > 0) ? concurrency : 2;
    std::vector<std::experimental::future<MyData *>> results;
    auto const chunk_size = (data.size() + nums_tasks - 1) / num_tasks;
    auto chunk_begin = data.begin();
    std::shared_ptr<std::atomic<bool>> done_flag = std::make_shared<std::atomic<bool>>(false);

    for (unsigned i = 0; i < nums_tasks; ++i) { // 启动num_tasks个异步任务
        auto chunk_end = (i < (nums_tasks - 1) ? chunk_begin + chunk_size : data.end());
        results.push_back(spawn_async([=]{ // 通过拷贝进行捕获，避免变量共享
            for (auto entry = chunk_begin; !*done_flag && (entry != chunk_end); ++entry) {
                if (matchs_find_criteria(*entry)) {
                    *done_flag = true;
                    return &*entry;
                }
            }
            return (MyData *)nullptr;
        }));
        chunk_begin = chunk_end;
    }

    std::shared_ptr<std::experimental::promise<FinnalResult>> final_result = std::make_shared<std::experimental::promise<FinalResult>>();
    struct DoneCheck {
        std::shared_ptr<std::experimental::promiss<FinalResult>> final_result;

        DoneCheck(std::shared_ptr<std::experimental::promise<FinalResult>> final_result_): final_result(std::move(final_result_)) {}

        void operator()(std::experimental::future<std::experimental::when_any_result<std::vector<std::experimental::future<MyData *>>>> results_param) {
            auto results = results_param.get();
            MyData *const ready_result = results.futures[results.index].get(); // 从完成的期望值中取值
            if (ready_result)
                final_result->set_value(process_found_value(*ready_result)); // 对最终结果赋值
            else { // 如果期望值不符合预期
                results.futures.erase(results.futures.begin() + results.index); // 从集合中丢弃完成的期望值
                if !(results.futures.empty()) {
                    std::experimental::when_any(results.futures.begin(), results.futures.end())
                        .then(std::move(*this)); // 继续检查
                } else { // 如果最终无法找到符合条件的期望值则抛出异常
                    final_result->set_exception(std::make_exception_ptr(std::runtime_error("Not found")));
                }
            }
        }
    };

    std::experimental::when_any(results.begin(), results.end())
        .then(DoneCheck(final_result)); // 一旦有任务完成立即执行DoneCheck
    return final_result->get_future();
}