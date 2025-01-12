#include "Map.h"

static int map[8][8] {
    { 1, 1, 1, 1, 1, 1, 1, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 1, 1, 1, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 1, 1, 1, 1, 1, 1, 1 },
};

static Player startingPoint = {{3, 3},{0, -1}};

int getTileInfo(vi2D tile) {
    return map[tile.y][tile.x];
}

int getMapMaxDistance() {
    return sizeof(map) / 4;
}

void setStartingPoint(Player &player) {
    player = startingPoint;
}
