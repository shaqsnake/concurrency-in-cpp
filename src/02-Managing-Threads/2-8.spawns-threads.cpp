#include <vector>
#include <thread>
#include <iostream>

void do_work(unsigned id) {
    std::cout << "Working at " << id << std::endl;
}

void f() {
    std::vector<std::thread> threads;
    for (unsigned i = 0; i < 20; i++) {
        threads.emplace_back(do_work, i);
    }

    for (auto& entry: threads)
        entry.join();
}

int main() {
    f();
    return 0;
}
