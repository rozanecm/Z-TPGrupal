#include <iostream>
#include <vector>
#include <pugixml.hpp>
#include <split.h>
#include <utility>
#include "ClientThread.h"
#include "commands/AddUnit.h"
#include "commands/RemoveUnit.h"
#include "commands/UpdatePosition.h"
#include "commands/LoadMap.h"
#include "commands/Update.h"
#include "commands/FactoryNextUnit.h"
#include "commands/AddBuilding.h"
#include "commands/AddNature.h"
#include "commands/StartGame.h"

void ClientThread::run() {
    initCommands();
    loop();
}

ClientThread::ClientThread(UnitsMonitor &unitsMonitor,
                           BuildingsMonitor &buildingsMonitor,
                           MapMonitor &mapMonitor,
                           ServerMessenger &messenger,
                           LobbyWindow& lobby,
                           GameWindow &window
) :
        unitsMonitor(unitsMonitor),
        buildingsMonitor(buildingsMonitor),
        mapMonitor(mapMonitor),
        messenger(messenger),
        lobby(lobby),
        window(window) {
}

void ClientThread::loop() {
    try {
        while (!finished) {
            std::string msg = messenger.receive();
            std::vector<std::string> commands = split(msg, '|');
            for (std::string &cmd : commands) {
                parse(cmd);
            }
        }
    } catch (SocketError &e) {
        return;
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
    std::cout << "Executing " << params[cmd] << std::endl;
    result->second->execute(args);
}

void ClientThread::finish() {
    finished = true;
    messenger.kill();
}

void ClientThread::initCommands() {
    commands["loadmap"] = new LoadMap(mapMonitor, buildingsMonitor, window);
    commands["addunit"] = new AddUnit(unitsMonitor);
    commands["removeunit"] = new RemoveUnit(unitsMonitor);
    commands["move"] = new UpdatePosition(unitsMonitor);
    commands["update"] = new Update(unitsMonitor);
    commands["nextunit"] = new FactoryNextUnit(window);
    commands["addbuilding"] = new AddBuilding(buildingsMonitor);
    commands["addnature"] = new AddNature();
    commands["startgame"] = new StartGame(messenger, lobby, window);
}

ClientThread::~ClientThread() {
    for (std::pair<std::string, Command *> c : commands) {
        delete c.second;
    }
}
