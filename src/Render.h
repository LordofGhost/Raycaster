
#ifndef RENDER_H
#define RENDER_H

#include "Main.h"
#include "Texture.h"

#include <SDL3/SDL_render.h>

enum {
    Decoration = 0,
    Enemy = 1,
};

struct Sprite {
    vd2D pos;
    int textureID;
    int type;
};

int draw3dSpace(SDL_Texture* pixelArray, Player &player);
void drawFlatTextures(Uint32* pixelArray, Player &player, int type);
void drawWall(Uint32* pixelArray, Player &player, double* distanceArray);
void drawSprites(Uint32* pixelArray, Player &player, double* distanceArray);
void applyShadow(Pixel &color);
int drawMiniMap(SDL_Renderer* renderer, Player &player);

#endif //RENDER_H
