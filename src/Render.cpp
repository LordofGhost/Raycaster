#include "Render.h"
#include "DDA.h"
#include "Map.h"

int draw3dSpace(SDL_Renderer* renderer, Player &player) {
    for (int renderColumn = 0; renderColumn < RENDER_WIDTH; renderColumn++) {
        /*
         * all the camera variables are absolute values to the grid
         * the cameraPlane is a vector that starts at left corner of camera and ends at the right corner
         * the cameraPos is the coordinate of the left corner of the camera
         */
        vd2D cameraPlane = {player.dir.y * 2, player.dir.x * 2};
        vd2D cameraPos = {player.pos.x - (cameraPlane.x * 0.5), player.pos.y - (cameraPlane.y * 0.5)};

        /*
         * all the column variables are relative to the camera position
         */
        double columnStepSizeOnCameraPlane = 1 / RENDER_WIDTH;
        double columnPosOnCameraPlane = columnStepSizeOnCameraPlane * renderColumn;
        vd2D columnPos = {cameraPos.x + (cameraPlane.x * columnPosOnCameraPlane), cameraPos.y + (cameraPlane.y * columnPosOnCameraPlane)};

        int tileColor;
        double wallDistance = dda(columnPos, player.dir, tileColor);
        switch (tileColor) {
            case 1:
                SDL_SetRenderDrawColor(renderer, 255,0,0,255);
                break;
            case 2:
                SDL_SetRenderDrawColor(renderer, 0,255,0,255);
            break;
            case 3:
                SDL_SetRenderDrawColor(renderer, 0,0,255,255);
            break;
            case 0:
                continue; // skip column if, it is out of map
        }

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
            if (getTileInfo({x, y}) != 0) {
                SDL_SetRenderDrawColor(renderer, 0,0,255,255);
            } else {
                SDL_SetRenderDrawColor(renderer, 0,100,255,255);
            }
            SDL_RenderPoint(renderer, x, y);
        }
    }
    SDL_SetRenderDrawColor(renderer, 255,255,255,255);
    SDL_RenderPoint(renderer, player.pos.x, player.pos.y);
    return 1;
}