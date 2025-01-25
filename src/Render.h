
#ifndef RENDER_H
#define RENDER_H

#include "Main.h"

#include <SDL3/SDL_render.h>

int draw3dSpace(SDL_Texture* pixelArray, Player &player);
void drawCeiling();
void drawFloor();
void drawWalls(Uint32* pixelArray, Player &player, int renderColumn, vd2D columnDirection, double columnAngle, int pitch);
void applyShadow(int* pixelColor);
int drawMiniMap(SDL_Renderer* renderer, Player &player);

#endif //RENDER_H
