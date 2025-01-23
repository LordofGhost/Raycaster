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

static Player startingPoint = {{3.2, 2.8},{0, -1}};

int getTileInfo(vi2D tile) {
    return map[tile.y][tile.x];
}

int getMapMaxDistance() {
    return sizeof(map) / 4;
}

vi2D getMatrixDimension() {
    int rows = sizeof(map) / sizeof(map[0]);
    int cols = sizeof(map[0]) / sizeof(map[0][0]);
    return {cols, rows};
}

void setStartingPoint(Player &player) {
    player = startingPoint;
}
