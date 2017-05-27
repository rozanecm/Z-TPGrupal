#include <iostream>
#include "cell.h"
#include "MapLoader.h"
#include "map.h"
#include "compass.h"
#include "unit.h"
#include "../socketError.h"

#define MAPLENGTH 15
int main (int argc, char **argv) {
    try {
        Server server_accepter(argv);

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
    return 0;
}

