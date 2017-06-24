//
// Created by rodian on 12/06/17.
//

#ifndef Z_TPGRUPAL_PLAYERINFO_H
#define Z_TPGRUPAL_PLAYERINFO_H


#include <messenger.h>
#include "factory.h"
#include "territory.h"

class PlayerInfo {
private:
    std::string id;
    Factory* fortress;
    int tech_level;
    Messenger* player_messenger;
    std::vector<Territory*> territories;

public:
    PlayerInfo(std::string id ,Factory* fortress);

    std::string getPlayerId() const;

    bool checkIfFortressLives();

    int getTechLevel();

    void increaseTechLevel();

    void decreaseTechLevel();

    Factory* getFortress();

    void addMessenger(Messenger* player_messenger);

    Messenger* getMessenger();

    void addTerritory(Territory* territory);

    void recalculateTechLevel();

    void eliminateThisTerritory(Territory* territory);

    ~PlayerInfo();
};


#endif //Z_TPGRUPAL_PLAYERINFO_H
