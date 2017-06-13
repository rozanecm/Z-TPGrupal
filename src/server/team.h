//
// Created by rodian on 12/06/17.
//

#ifndef Z_TPGRUPAL_TEAM_H
#define Z_TPGRUPAL_TEAM_H


#include <vector>
#include "playerInfo.h"

class Team {
private:
    std::vector<PlayerInfo> players;
    int team_id;
public:
    Team(std::vector<PlayerInfo> players,int team_id);

    bool doesTeamLose();

    std::vector<PlayerInfo>& getPlayersInfo();
};


#endif //Z_TPGRUPAL_TEAM_H
