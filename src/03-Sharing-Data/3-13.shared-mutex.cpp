#include <map>
#include <shared_mutex>
#include <mutex>
#include <iostream>
#include <thread>
#include <chrono>

class dns_entry {
    std::string ip_addr;

public:
    dns_entry() = default;
    dns_entry(std::string const& ip): ip_addr(ip) {}

    std::string get_ip_addr() {
        return ip_addr;
    }
};

class dns_cache {
    std::map<std::string, dns_entry> entries;
    mutable std::shared_mutex entry_mutex;

public:
    dns_entry find_entry(std::string const& domain) const {
        std::shared_lock<std::shared_mutex> lk(entry_mutex); // 共享锁，保护读操作
        std::map<std::string, dns_entry>::const_iterator const it = entries.find(domain);
        return (it == entries.end()) ? dns_entry("0.0.0.0") : it->second;
    }

    void update_or_add_entry(std::string const& domain, dns_entry const& dns_details) {
        std::lock_guard<std::shared_mutex> lk(entry_mutex); // 互斥锁，保护写操作
        entries[domain] = dns_details;
    }
};

void get_value(dns_cache& dns, std::string key) {
    std::cout << dns.find_entry(key).get_ip_addr() << std::endl;
}

int main() {
    dns_cache dns{};
    dns.update_or_add_entry("www.google.com", dns_entry("8.8.8.8"));
    std::cout << dns.find_entry("www.google.com").get_ip_addr() << std::endl;

    std::thread t1(get_value, std::ref(dns), "www.google.com");
    std::thread t2(&dns_cache::update_or_add_entry, &dns, "www.google.com", dns_entry("7.7.7.7"));

    // std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    std::thread t3(get_value, std::ref(dns), "www.google.com");

    t1.join();
    t2.join();
    t3.join();

    return 0;
}