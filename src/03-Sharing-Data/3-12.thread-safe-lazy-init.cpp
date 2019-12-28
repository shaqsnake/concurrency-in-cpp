#include <mutex>
#include <iostream>
#include <thread>

struct data_packet {
    int data;
    data_packet(int data_): data(data_) {}
};

struct connection_info {};

struct connection_handle {
    connection_handle() {}

    void send_data(data_packet data) {
        std::cout << "Sending data ..." << std::endl;
    }

    data_packet receive_data() { 
        std::cout << "Receiving data ..." << std::endl;
        return data_packet{0};
    }
};

struct connection_manager {
    static connection_handle open(connection_info) {
        std::cout << "Open connection ..." << std::endl;
        return connection_handle{};
    }
};

class X {
private:
    connection_info connection_details;
    connection_handle connection;
    std::once_flag connection_init_flag;

    void open_connection() {
        connection = connection_manager::open(connection_details);
    }

public:
    X(connection_info const& connection_details_): connection_details(connection_details_) {}

    void send_data(data_packet const& data) {
        std::call_once(connection_init_flag, &X::open_connection, this); // std::call_once保证不同线程也只会调用X::open_connection一次
        connection.send_data(data);
    }

    data_packet receive_data() {
        std::call_once(connection_init_flag, &X::open_connection, this); // 与std::thread类似，调用成员函数时，需要传入成员函数地址和指针
        return connection.receive_data();
    }

    void test() {}
};

int main() {
    connection_info conn_info;
    X x(conn_info);
    data_packet data(10);

    std::thread t1(&X::send_data, &x, data);
    std::thread t2(&X::receive_data, &x);

    t1.join();
    t2.join();

    return 0;
}