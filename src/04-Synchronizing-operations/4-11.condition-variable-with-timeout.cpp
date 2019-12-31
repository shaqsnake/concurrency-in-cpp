#include <condition_variable>
#include <mutex>
#include <iostream>

std::condition_variable cv;
bool done;
std::mutex m;

bool wait_loop() {
    auto const timeout = std::chrono::steady_clock::now() + std::chrono::milliseconds(500);
    std::unique_lock<std::mutex> lock(m);
    while (!done) {
        if (cv.wait_until(lock, timeout) == std::cv_status::timeout)
            break;
    }

    return done;
}

int main() {
    std::cout << wait_loop() << std::endl;

    return 0;
}