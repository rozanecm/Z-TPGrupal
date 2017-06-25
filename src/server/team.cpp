//
// Created by rodian on 12/06/17.
//

#include "team.h"

Team::Team(std::vector<PlayerInfo>& players, int team_id) :
        players(players), team_id(team_id) {}

bool Team::doesTeamLose() {
    bool lose = true;
    // if any of the fortress of the team is alive, they didn't lose
    for (auto& p: players) {
        if (p.checkIfFortressLives()) {
            lose = false;
        }
        if (!p.areYouStillConected()) {
            lose = true;
        }
    }
    return lose;
}

std::vector<PlayerInfo> &Team::getPlayersInfo() {
    return this->players;
}
