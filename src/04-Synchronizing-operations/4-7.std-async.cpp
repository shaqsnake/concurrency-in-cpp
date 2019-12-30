#include <string>
#include <future>
#include <iostream>

struct X {
    void foo(int x, std::string const& s) {
        std::cout << "X::foo(" << x << ", " << s << ")" << std::endl;
    }

    std::string bar(std::string const& s) {
        return s;
    }
};

struct Y {
    double operator()(double d) {
        return d;
    }
};

X baz(X& x) {
    return x;
}

class move_only {
public:
    move_only() = default;
    move_only(move_only&&) = default;
    move_only(move_only const&) = delete;
    move_only& operator=(move_only&&) = default;
    move_only& operator=(move_only const&) = delete;

    void operator()() {
        std::cout << "move_olny()" << std::endl;
    }
};

int main() {
    X x;
    auto f1 = std::async(&X::foo, &x, 42, "hello"); // 调用p->foo(42, "hello"), p = &x
    auto f2 = std::async(&X::bar, x, "goodbye"); // 调用tmpx.bar("goodbye"), tmpx是x的拷贝副本
    std::cout << f2.get() << std::endl;

    Y y;
    auto f3 = std::async(Y(), 3.141); // 调用tmpy(3.141), tmpy通过Y的移动构成函数生成
    auto f4 = std::async(std::ref(y), 2.718); // 调用y(2.718)
    std::cout << f3.get() << std::endl;
    std::cout << f4.get() << std::endl;

    std::async(baz, std::ref(x)); // 调用baz(x)

    auto f5 = std::async(move_only()); // 调用tmp(), tmp是通过std::move(move_only())构造而成

    auto f6 = std::async(std::launch::async, Y(), 1.2); // 在新线程上执行
    std::cout << f6.get() << std::endl;
    auto f7 = std::async(std::launch::deferred, baz, std::ref(x)); // 在wait()或get()调用时执行
    auto f8 = std::async(std::launch::deferred | std::launch::async, baz, std::ref(x)); // 可选择执行方式
    f7.wait(); // 调用延迟函数

    return 0;
}