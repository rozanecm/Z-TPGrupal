#include <iostream>
#include "ClientThread.h"

void ClientThread::run() {
    while (true){
        /* initialize map so then can be completed with read data */
        mapMonitor.initializeMap(200, 200);

        for (int i = 0; i < 200; i++){
            for (int j = 0; j < 200; j++){
                if (i%2 == 0){
                    mapMonitor.setCell(i, j, "Agua");
                } else{
                    mapMonitor.setCell(i, j, "Tierra");
                }
            }
        }
        std::cout<<"hi"<<std::endl;
        break;
    }
}

ClientThread::ClientThread(PlayersMonitor &playerMonitor,
                           BuildingsMonitor &buildingsMonitor,
                           MapMonitor &mapMonitor) :
        playersMonitor(playerMonitor),
        buildingsMonitor(buildingsMonitor),
        mapMonitor(mapMonitor){
}
