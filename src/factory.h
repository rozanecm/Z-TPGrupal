//
// Created by rodian on 22/05/17.
//

#ifndef Z_TPGRUPAL_FACTORY_H
#define Z_TPGRUPAL_FACTORY_H


#include "Occupant.h"
#include "teamable.h"

class Factory: public Occupant, public Teamable {
protected:
    bool running;
    int tech_level;

public:
    Factory(int life, std::string type, Size position);

    Factory(int life, std::string type, std::string team, Size position);

    virtual void build() = 0;

    virtual void stopBuilding() = 0;

    virtual int getSelectedUnitTime() = 0;

    void setNewPlayer(std::string player, int tech_level);
};


#endif //Z_TPGRUPAL_FACTORY_H
