#ifndef Z_TPGRUPAL_NATURE_H
#define Z_TPGRUPAL_NATURE_H


#include <utility>
#include "enums/NatureEnum.h"

class Nature {
    NatureEnum type;
    std::pair<unsigned int, unsigned int> position;
    int id;

public:
    Nature(NatureEnum type, std::pair<unsigned int, unsigned int> position,
               int id);

    std::pair<unsigned int, unsigned int> getPosition();

    NatureEnum getType();
};


#endif //Z_TPGRUPAL_NATURE_H
