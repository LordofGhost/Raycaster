#include "Map.h"

#include <iostream>

static int floorMap[MAP_SIZE_Y][MAP_SIZE_X] {
    { 1, 1, 1, 1, 1, 1, 1, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 1, 1, 1, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 1, 1, 1, 1, 1, 1, 1 },
};

static int wallMap[MAP_SIZE_Y][MAP_SIZE_X] {
    { 1, 1, 1, 1, 1, 1, 1, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 1, 1, 1, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 1, 1, 1, 1, 1, 1, 1 },
};

static int ceilingMap[MAP_SIZE_Y][MAP_SIZE_X] {
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

int getTileInfo(int type, vi2D tile) {
    if (type != FLOOR && type != WALL && type != CEILING) {
        std::cerr << "getTileInfo: invalid map type" << std::endl;
        return 0;
    }

    if (tile.x > MAP_SIZE_X - 1 || tile.y > MAP_SIZE_Y - 1) {
        std::cerr << "getTileInfo: trying to read outside of map" << std::endl;
        return 0;
    }

    switch (type) {
        case FLOOR:
            return floorMap[tile.y][tile.x];
        case WALL:
            return wallMap[tile.y][tile.x];
        case CEILING:
            return ceilingMap[tile.y][tile.x];
    }
}

int getMapMaxDistance() {
    return sizeof(wallMap) / 4;
}

vi2D getMatrixDimension() {
    int rows = sizeof(wallMap) / sizeof(wallMap[0]);
    int cols = sizeof(wallMap[0]) / sizeof(wallMap[0][0]);
    return {cols, rows};
}

void setStartingPoint(Player &player) {
    player = startingPoint;
}
