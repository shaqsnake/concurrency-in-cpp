#include <future>

void process_connections(connection_set& connections) {
    while (done(connections)) { // 处理所有连接
        for (connection_iterator connection = connections.begin(), end = connections.end(); connection != end; ++connection) { // 依次轮询每个连接
            if (connection->has_incoming_data()) { // 有数据接收
                data_packet data = connection->incoming();
                std::promise<payload_type>& p = connection->get_promise(data.id); // 把数据id映射到一个std::promise上
                p.set_value(data.payload);
            }

            if (connection->has_outgoing_data()) { // 有数据发送
                outgoing_date data = connection->top_of_outgoing_queue();
                connection->send(data.payload);
                data.promise.set_value(true); // 发送后将承诺值设为true，表示发送成功
            }
        }
    }
}