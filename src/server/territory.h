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
    Teamable flag;
    Size territory_size;
    int id;

public:
    Territory(const std::map<int, Factory *> &factories, Position flag_position,
                  Size territory_size, int id);

    void grabFlag(std::string& new_team);

    int getTechLevel();

    void changeFactoriesTechLevel(int tech_level);

    Teamable* getFlag();

    std::map<int,Factory*>& getFactories();

    bool doesTerritorysOwnerChanged();

    int getId();

    ~Territory();
};


#endif //Z_TPGRUPAL_TERRITORY_H
