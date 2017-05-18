#include <fstream>
#include "MapGenerator.h"

MapGenerator::MapGenerator(int size) :
    size(size)
{
}

void MapGenerator::generate(const std::string& name) {
    std::string path = "maps/" + name + ".xml";
    output.open(path, std::ios_base::out);
    output << "Get Bent";
}

MapGenerator::~MapGenerator() {
    if (output.is_open()) {
        output.close();
    }
}



