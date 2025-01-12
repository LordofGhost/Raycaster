#include "Map.h"

static int map[8][8] {
    { 3, 3, 3, 3, 3, 3, 3, 3 },
    { 1, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 2, 2, 2, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 1 },
    { 3, 3, 3, 3, 3, 3, 3, 3 },
};

static Player startingPoint = {{3.2, 2.8},{0, -1}};

int getTileInfo(vi2D tile) {
    return map[tile.y][tile.x];
}

int getMapMaxDistance() {
    return sizeof(map) / 4;
}

void setStartingPoint(Player &player) {
    player = startingPoint;
}
