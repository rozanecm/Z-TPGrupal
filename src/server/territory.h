//
// Created by rodian on 10/06/17.
//

#ifndef Z_TPGRUPAL_TERRITORY_H
#define Z_TPGRUPAL_TERRITORY_H


#include <vector>
#include "factory.h"
#include "teamable.h"

class Territory: public Teamable {
private:
    std::map<int,Factory*> factories;
    Position flag_position;
    Size territory_size;

public:
    Territory(const std::map<int,Factory*>& factories,
              Position flag_position, Size territory_size);

    void grabFlag(std::string& new_team);

    int getTechLevel();

    void changeFactoriesTechLevel(int tech_level);

    Position getFlagPosition();

    std::map<int,Factory*>& getFactories();

    bool doesTerritorysOwnerChanged();
};


#endif //Z_TPGRUPAL_TERRITORY_H
