#include <iostream>
#include <string>
#include <vector>
#include <pugixml.hpp>
#include <split.h>
#include <utility>
#include "ClientThread.h"
#include "ServerMessenger.h"
#include "commands/Command.h"
#include "commands/AddUnit.h"
#include "commands/RemoveUnit.h"
#include "commands/UpdatePosition.h"

#define MAP "maps/map.xml" // temporary

void ClientThread::run() {
    std::string map(MAP);
    initMap(map);
    initCommands();
    loop();
}

void ClientThread::initMap(std::string& path) const {
    /* initialize map so then can be completed with read data */
    mapMonitor.initializeMap(5, 5);

    std::vector<std::vector<std::__cxx11::string>> map;
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(path.c_str());
    if (!result) {
        std::cerr << "FATAL ERROR LOADING MAP: " << result.description();
        return;
    }

    pugi::xml_node terrain = doc.child("Map").child("Terrain");
    for (auto node_row : terrain.children()) {
        std::vector<std::__cxx11::string> row;
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
                           MapMonitor &mapMonitor,
                           const char *addr, unsigned int port) :
        playersMonitor(playerMonitor),
        buildingsMonitor(buildingsMonitor),
        mapMonitor(mapMonitor),
        messenger(addr, port)
{
}

void ClientThread::loop() {
    while (!finished) {
        std::string msg = messenger.receive();
        if (msg == "") {
            continue;
        }
        parse(msg);
    }
}

void ClientThread::parse(std::string &s) {
    std::vector<std::string> params = split(s, '-');
    int cmd = 0;
    auto result = commands.find(params[cmd]);
    if (result == commands.end()) {
        std::cerr << "Invalid command: " << params[cmd] << std::endl;
        return;
    }
    std::vector<std::string> args(++params.begin(), params.end());
    result->second->execute(args);
}

void ClientThread::finish() {
    finished = true;
    messenger.kill();
}

void ClientThread::initCommands() {
    commands["addunit"] = new AddUnit(playersMonitor);
    commands["removeunit"] = new RemoveUnit(playersMonitor);
    commands["move"] = new UpdatePosition(playersMonitor);
}

ClientThread::~ClientThread() {
    for (std::pair<std::string, Command*> c : commands) {
        delete c.second;
    }
}
