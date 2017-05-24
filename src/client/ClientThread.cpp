#include <iostream>
#include <string>
#include <vector>
#include "ClientThread.h"
#include "../../libs/xml/pugixml.hpp"

#define MAP "maps/map.xml" // temporary

void ClientThread::run() {
    /* initialize map so then can be completed with read data */
    mapMonitor.initializeMap(5, 5);

    std::vector<std::vector<std::string>> map;
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(MAP);
    if (!result) {
        std::cerr << "FATAL ERROR LOADING MAP: " << result.description();
        return;
    }

    pugi::xml_node terrain = doc.child("Map").child("Terrain");
    std::cout << terrain.name();
    for (auto node_row : terrain.children()) {
        std::vector<std::string> row;
        for (auto cell : node_row.children()) {
            row.push_back(cell.attribute("terrain").value());
        }
        map.push_back(row);
    }

    unsigned long size = map.size();
    mapMonitor.initializeMap(size, size);
    for (int i = 0; i < size; i++){
        for (int j = 0; j < size; j++){
            mapMonitor.setCell(i, j, map[i][j]);
        }
    }
}

ClientThread::ClientThread(PlayersMonitor &playerMonitor,
                           BuildingsMonitor &buildingsMonitor,
                           MapMonitor &mapMonitor) :
        playersMonitor(playerMonitor),
        buildingsMonitor(buildingsMonitor),
        mapMonitor(mapMonitor){
}
