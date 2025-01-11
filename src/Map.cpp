#include "Map.h"
#include <SDL3/SDL.h>

int map[8][8] {
    { 1, 1, 1, 1, 1, 1, 1, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 1, 1, 1, 1, 1, 1, 1 },
};

int getTileInfo(int x, int y) {
    return map[y][x];
}

int drawMiniMap(SDL_Renderer* renderer, Player player) {
    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            if (map[y][x]) {
                SDL_SetRenderDrawColor(renderer, 0,0,255,255);
            } else {
                SDL_SetRenderDrawColor(renderer, 0,100,255,255);
            }
            SDL_RenderPoint(renderer, x, y);
        }
    }
    SDL_SetRenderDrawColor(renderer, 255,255,255,255);
    SDL_RenderPoint(renderer, player.pos.x, player.pos.y);
}