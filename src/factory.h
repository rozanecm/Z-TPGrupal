//
// Created by rodian on 22/05/17.
//

#ifndef Z_TPGRUPAL_FACTORY_H
#define Z_TPGRUPAL_FACTORY_H


#include "Occupant.h"

class Factory: public Occupant {
protected:
    bool running;
    std::string player;
    int tech_level;

public:
    Factory(int life, std::string type, Size position);

    virtual void build() = 0;

    virtual void stopBuilding() = 0;

    virtual int getSelectedUnitTime() = 0;

    // Returns the id of the player who is in control of the Factory
    std::string getPlayer();

    void setNewPlayer(std::string player, int tech_level);
};


#endif //Z_TPGRUPAL_FACTORY_H
