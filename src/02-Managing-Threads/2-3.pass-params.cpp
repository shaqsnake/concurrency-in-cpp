#include <string>
#include <thread>
#include <iostream>
#include <chrono>

void f(int i, std::string const& s)
{
    std::cout << "i = " << i << "; s = " << s << std::endl;
}

void oops(int some_param)
{
    char buffer[1024];
    sprintf(buffer, "%i", some_param);
    // std::thread t(f, 3, buffer); // 这里buffer可能会在转换成string前被销毁（函数结束）
    // std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    std::thread t(f, 3, std::string(buffer));  // 使用显示转换，避免buffer指针悬垂
    t.detach();
}

// ------

struct widget_id {};
struct widget_data {};
void update_data_for_widget(widget_id id, widget_data& data)
{}
void oops_again(widget_id w)
{
    widget_data data;
    std::thread t(update_data_for_widget, w, std::ref(data));  // 不用std::ref会在std::thread构造时拷贝data，最后把data当作右值看待，导致无法编译
    t.join();
}

// ------

class X {
public:
    void do_lengthy_work(int) {
        std::cout << "do_lengthy_work!" << std::endl; 
    }
};

// -----

class big_object {
public:
    void prepare_data(int x) { data = x; }
    int get_data() { return data; }

private:
    int data;
};

void process_big_object(std::unique_ptr<big_object> ptr) {
    std::cout << ptr->get_data() << std::endl;
}


int main()
{
    oops(15);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    X my_x;
    std::thread t(&X::do_lengthy_work, &my_x, 3); // 传递一个成员函数指针给线程构造函数，同时my_x的地址作为对象指针提供给成员函数
    t.join();

    std::unique_ptr<big_object> p(new big_object);
    p->prepare_data(42);
    std::thread my_thread(process_big_object, std::move(p)); // 因为std::unique_ptr同一时间只能有一个引用，
                                                             // 而在线程构造时会执行拷贝的动作，
                                                             // 所以这里要用std::move把对象所有权转移给线程内部，才可以通过编译
    my_thread.join();

    return 0;
}