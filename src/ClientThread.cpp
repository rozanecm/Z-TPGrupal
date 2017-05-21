#include "ClientThread.h"

void ClientThread::run() {

}

ClientThread::ClientThread(PlayersMonitor &playerMonitor,
                           BuildingsMonitor &buildingsMonitor) :
        playersMonitor(playerMonitor),
        buildingsMonitor(buildingsMonitor){

}
