#include <iostream>
#include <pugixml.hpp>
#include "server.h"

#define PORT 8000

unsigned int load_port(const char* cfg_file_path) {
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(cfg_file_path);
    if (!result) {
        std::cout << "Error reading cfg file: " << result.description()
                  << std::endl;
        return 0;
    }
    pugi::xml_node port_node = doc.child("Config").child("Port");
    std::string port = port_node.attribute("port").value();
    if (!port.size()) {
        std::cout << "Error reading port from cfg file!" << std::endl;
        return 0;
    }
    int a = std::stoi(port);
    return (unsigned int) a;
}

int main (int argc, char **argv) {
    std::string cfg = argv[1];
    if (!cfg.size()) {
        std::cout << "Error loading cfg file" << std::endl;
        return 1;
    }
    unsigned int port = load_port(argv[1]);
    if (!port) {
        return 1;
    }

    try {
        std::mutex m;

        Menu menu(m);

        Server server_accepter(port, menu);
        server_accepter.start();
        char exit = 'a';

        while (exit != 'q') {
            std::cin>>exit;
        }

        server_accepter.stop();
        server_accepter.join();

        return 0;
    } catch (SocketError& e) {
        std::cout<< e.what();
        return 1;
    }
}

