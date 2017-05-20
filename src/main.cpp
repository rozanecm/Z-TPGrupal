#include "MapGenerator.h"

int main (int argc, char **argv)
{

    MapGenerator generator(10, 10, 30, 0);
    generator.generate("map");

}
