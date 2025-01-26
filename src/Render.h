
#ifndef RENDER_H
#define RENDER_H

#include "Main.h"
#include "Texture.h"

#include <SDL3/SDL_render.h>

struct color {

};

int draw3dSpace(SDL_Texture* pixelArray, Player &player);
void drawFloor(Uint32* pixelArray, int &renderColumn);
void drawWalls(Uint32* pixelArray, Player &player, int &renderColumn, vd2D &columnDirection, double &columnAngle);
void applyShadow(Pixel &color);
int drawMiniMap(SDL_Renderer* renderer, Player &player);

#endif //RENDER_H
