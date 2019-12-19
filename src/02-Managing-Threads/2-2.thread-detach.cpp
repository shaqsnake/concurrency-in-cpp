#include <string>
#include <iostream>
#include <thread>

int COUNT = 0;

enum cmdType {open_new_document, close_document};

struct user_command {
    cmdType type;
};

void open_document_and_display_gui(std::string const& filename) {
    std::cout << "Open " << filename << std::endl;
}

bool done_editing() {
    return COUNT == 10;
}

user_command get_user_input() {
    return user_command {
        open_new_document
    };
}

std::string get_filename_from_user() {
    COUNT++;
    return "New file " + std::to_string(COUNT);
}

void process_user_input(user_command cmd) {
    std::cout << "Processing " << cmd.type << std::endl;
}

void edit_document(std::string const& filename) {
    open_document_and_display_gui(filename);
    while (!done_editing()) {
        user_command cmd = get_user_input();
        if (cmd.type == open_new_document) {
            std::string const new_filename = get_filename_from_user();
            std::thread t(edit_document, new_filename); // 启动新线程
            t.detach(); // 分离线程
        } else {
            process_user_input(cmd);
        }
    }
}

int main() {
    edit_document("My file");
    return 0;
}