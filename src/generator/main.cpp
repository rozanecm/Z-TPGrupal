#include <iostream>
#include "MapGenerator.h"

#define SIZE 1
#define WATER_PCT 2
#define LAVA_PCT 3
#define TERRITORIES 4
#define NAME 5

int main(int argc, char **argv) {
    if (argc < 6) {
        std::cout << "Usage: " << argv[0] << " <size> <water_pct> <lava_pct> "
                "<territories_amt> <name>" << std::endl;
        return 1;
    }

    try {
        int size = std::stoi(argv[SIZE]),
        water = std::stoi(argv[WATER_PCT]),
        lava = std::stoi(argv[LAVA_PCT]),
        territories = std::stoi(argv[TERRITORIES]);
        std::string name = argv[NAME];
        MapGenerator generator(size, lava, water, territories);
        generator.generate(name);
    } catch (const std::invalid_argument& e) {
        std::cout << "Usage: " << argv[0] << " <size> <water_pct> <lava_pct> "
                "<territories_amt> <name>" << std::endl;
        return 1;
    }
}
