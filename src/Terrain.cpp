//
// Created by rodian on 13/05/17.
//

#include "Terrain.h"

Terrain::Terrain(std::string &kind, int factor) :
                        kind(kind), factor(factor) {}

std::string Terrain::getKind() const{
    return this->kind;
}

int Terrain::getFactor() const {
    return this->factor;
}

Terrain::~Terrain() {}



