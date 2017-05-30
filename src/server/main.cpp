#include <iostream>
#include "cell.h"
#include "compass.h"
#include "server.h"
#include "../common/socketError.h"

int main (int argc, char **argv) {

    try {
        std::mutex m;

        Menu menu(m);

        Server server_accepter(argv, menu);

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

