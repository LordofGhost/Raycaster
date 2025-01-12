#include "Render.h"
#include "DDA.h"
#include "Map.h"
#include "math.h"

int draw3dSpace(SDL_Renderer* renderer, Player &player) {
    for (int renderColumn = 0; renderColumn < RENDER_WIDTH; renderColumn++) {
        double columnStepSize = (double)RENDER_FOV / (double)RENDER_WIDTH;
        double columnAngle = (columnStepSize * renderColumn) - (RENDER_FOV / 2);
        vd2D columnDirection = rotateVector(player.dir, degreeToRad(columnAngle));
        double angleDifference = abs(degreeToRad(columnAngle));

        int tileColor;
        bool shadowSide;
        double wallDistance = dda(player.pos, columnDirection, tileColor, shadowSide) * cos(angleDifference);
        if (tileColor != 0) setRenderColor(renderer, tileColor, shadowSide); else continue; // skip column if, it is out of map

        // making the height of wall proportional to the screen height, world scale is already 1
        int distanceToHeight = RENDER_HEIGHT / wallDistance;
        // Prevent from drawing outside the renderer
        distanceToHeight > RENDER_HEIGHT ? distanceToHeight = RENDER_HEIGHT : distanceToHeight;
        // calculating both points by going in the middle of the screen and then go up or down
        int yTop = (RENDER_HEIGHT / 2) - (distanceToHeight / 2);
        int yBottom = (RENDER_HEIGHT / 2) + (distanceToHeight / 2);
        SDL_RenderLine(renderer, renderColumn, yTop, renderColumn, yBottom);
    }
    return 1;
}

int drawMiniMap(SDL_Renderer* renderer, Player &player) {
    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            setRenderColor(renderer, getTileInfo({x, y}), false);
            SDL_FRect rect;
            rect.x = x * MINI_MAP_SCALE;
            rect.y = y * MINI_MAP_SCALE;
            rect.w = MINI_MAP_SCALE;
            rect.h = MINI_MAP_SCALE;
            SDL_RenderFillRect(renderer, &rect);
        }
    }
    int tileInfo;
    bool shadowSide;
    double distance = dda(player.pos, player.dir, tileInfo, shadowSide);
    SDL_SetRenderDrawColor(renderer, 255,200,255,255);
    SDL_RenderLine(renderer, player.pos.x * MINI_MAP_SCALE, player.pos.y * MINI_MAP_SCALE, (player.pos.x + (player.dir.x * distance)) * MINI_MAP_SCALE, (player.pos.y + (player.dir.y * distance)) * MINI_MAP_SCALE);
    SDL_SetRenderDrawColor(renderer, 255,255,255,255);
    SDL_RenderPoint(renderer, player.pos.x * MINI_MAP_SCALE, player.pos.y * MINI_MAP_SCALE);
    return 1;
}

void setRenderColor(SDL_Renderer* renderer, int tileColor, bool shadow) {
    switch (tileColor) {
        case 0:
            SDL_SetRenderDrawColor(renderer, 0,0,0,255);
            break;
        case 1:
            shadow ? SDL_SetRenderDrawColor(renderer, 170,0,0,255) : SDL_SetRenderDrawColor(renderer, 255,0,0,255);
        break;
        case 2:
            shadow ? SDL_SetRenderDrawColor(renderer, 0,170,0,255) : SDL_SetRenderDrawColor(renderer, 0,255,0,255);
        break;
        case 3:
            shadow ? SDL_SetRenderDrawColor(renderer, 0,0,170,255) : SDL_SetRenderDrawColor(renderer, 0,0,255,255);
        break;
    }
}