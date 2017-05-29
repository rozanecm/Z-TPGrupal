#include "MapGenerator.h"

int main (int argc, char **argv)
{
    int size = 100,
    lava = 5,
    water = 10,
    variance = 0;
    MapGenerator generator(size, lava, water, variance);
    generator.generate("map");

}
