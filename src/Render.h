
#ifndef RENDER_H
#define RENDER_H

#include "Main.h"

#include <SDL3/SDL_render.h>

int draw3dSpace(SDL_Texture* pixelArray, Player &player);
int drawMiniMap(SDL_Renderer* renderer, Player &player);

#endif //RENDER_H
