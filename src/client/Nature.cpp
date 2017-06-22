#include "Nature.h"

Nature::Nature(NatureEnum type, std::pair<unsigned int, unsigned int> position,
               int id) :
        type(type), position(position), id(id){
}

std::pair<unsigned int, unsigned int> Nature::getPosition() {
    return position;
}

NatureEnum Nature::getType() {
    return type;
}
