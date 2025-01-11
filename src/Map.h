
#ifndef MAP_H
#define MAP_H
#include <SDL3/SDL_render.h>
#include "Main.h"

int getTileInfo(vi2D);
int getMapMaxDistance();
int drawMiniMap(SDL_Renderer* renderer, Player player);

#endif //MAP_H
