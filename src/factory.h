//
// Created by rodian on 22/05/17.
//

#ifndef Z_TPGRUPAL_FACTORY_H
#define Z_TPGRUPAL_FACTORY_H


#include "Occupant.h"

class Factory: public Occupant {
protected:
    bool running;
public:
    Factory(int life, std::string type, Size position);

    virtual void build() = 0;

    virtual void stopBuilding() = 0;

    virtual int getSelectedUnitTime() = 0;
};


#endif //Z_TPGRUPAL_FACTORY_H
