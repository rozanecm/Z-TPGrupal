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
#include "commands/LoadMap.h"

void ClientThread::run() {
    initCommands();
    loop();
}

ClientThread::ClientThread(PlayersMonitor &playerMonitor,
                           BuildingsMonitor &buildingsMonitor,
                           MapMonitor &mapMonitor,
                           ServerMessenger& messenger) :
        playersMonitor(playerMonitor),
        buildingsMonitor(buildingsMonitor),
        mapMonitor(mapMonitor),
        messenger(messenger)
{
}

void ClientThread::loop() {
    messenger.send("ready");
    while (!finished) {
        std::string msg = messenger.receive();
        std::cout<<msg<<std::endl;
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
    commands["loadmap"] = new LoadMap(mapMonitor);
    commands["addunit"] = new AddUnit(playersMonitor);
    commands["removeunit"] = new RemoveUnit(playersMonitor);
    commands["move"] = new UpdatePosition(playersMonitor);
}

ClientThread::~ClientThread() {
    for (std::pair<std::string, Command*> c : commands) {
        delete c.second;
    }
}
