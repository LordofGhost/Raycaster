#include "Render.h"
#include "DDA.h"
#include "Map.h"
#include <math.h>

int draw3dSpace(SDL_Texture* buffer, Player &player) {

    // load the buffer in the RAM and get access to it
    void* pixels; int pitch; // the pitch is not needed but can't be replaced by a nullptr
    SDL_LockTexture(buffer, NULL, &pixels, &pitch);
    // memory layout: FF(alpha) FF(red) FF(green) FF(blue)
    Uint32* pixelArray = (Uint32*)pixels;

    drawFlatTextures(pixelArray, player, CEILING);
    drawFlatTextures(pixelArray, player, FLOOR);
    drawWall(pixelArray, player);

    // Shift buffer from RAM to the GPU
    SDL_UnlockTexture(buffer);

    return 1;
}

void drawFlatTextures(Uint32* pixelArray, Player &player, int type) {

    for (int renderRow = 0; renderRow < RENDER_HEIGHT/2; renderRow++) {
        // calculate the view angle of camera to the row in the ceiling using the FOV and aspect ratio of the screen
        // (60/16)* 9 / 2 = 17
        double rowAngle = 17 * (renderRow / double(RENDER_HEIGHT/2)) + 73;

        // the distance from the player to the current row, where 0.5 is the height of the player in the world space
        double distanceToRow = tan(degreeToRad(rowAngle)) * 0.5;

        for (int pixel = 0; pixel < RENDER_WIDTH; pixel++) {
            Pixel color = {0,0,0};

            // the pixel angle is the relative angle from the left corner of the row to the current pixel
            double pixelAngle = (RENDER_FOV / (double)RENDER_WIDTH) * pixel - (RENDER_FOV / 2);;

            // the individual distance from the player to the pixel to remove the fish eye effect
            double pixelDistance = distanceToRow / cos(degreeToRad(abs(pixelAngle)));

            // current vector from the player to the pixel
            vd2D pixelVector = rotateVector({player.dir.x * pixelDistance, player.dir.y * pixelDistance}, degreeToRad(pixelAngle));

            // apply the player position to make the position absolute
            vd2D pixelPosition;
            pixelPosition.x = player.pos.x + pixelVector.x;
            pixelPosition.y = player.pos.y + pixelVector.y;

            vi2D textureCoordinate;
            textureCoordinate.x = (pixelPosition.x - floor(pixelPosition.x)) * TEXTURE_SIZE;
            textureCoordinate.y = (pixelPosition.y - floor(pixelPosition.y)) * TEXTURE_SIZE;



            if (type == FLOOR) {
                int tileTextureID = getTileInfo(FLOOR,{(int)pixelPosition.x, (int)pixelPosition.y});
                getTextureColor(tileTextureID, textureCoordinate, color);
                pixelArray[pixel + RENDER_WIDTH * RENDER_HEIGHT - (RENDER_WIDTH * renderRow)] = color.b + (color.g << 8) + (color.r << 16) + (255 << 24);
            } else if (type == CEILING) {
                int tileTextureID = getTileInfo(CEILING,{(int)pixelPosition.x, (int)pixelPosition.y});
                getTextureColor(tileTextureID, textureCoordinate, color);
                pixelArray[pixel + RENDER_WIDTH * renderRow] = color.b + (color.g << 8) + (color.r << 16) + (255 << 24);
            }
        }
    }
}

void drawWall(Uint32* pixelArray, Player &player) {

    for (int renderColumn = 0; renderColumn < RENDER_WIDTH; renderColumn++) {

        // calculating the pixel column specific units
        double columnAngle = ((double)RENDER_FOV / (double)RENDER_WIDTH * renderColumn) - (RENDER_FOV / 2);
        vd2D columnDirection = rotateVector(player.dir, degreeToRad(columnAngle));

        // get the distance to the next wall in the given direction
        int tileTextureID;
        bool hitOnAxisX;
        double wallDistance = dda(player.pos, columnDirection, tileTextureID, hitOnAxisX);
        // the cos value is needed to remove the fisheye effect
        double columnAngleDifference = abs(degreeToRad(columnAngle));
        double wallDistanceNoFishEye =  wallDistance * cos(columnAngleDifference);

        // calculate the position where the ray hits the wall to correctly map the texture
        double wallHitPosition;
        if (hitOnAxisX) {
            // the wall goes along the x-axis
            wallHitPosition = player.pos.y + (columnDirection.y * wallDistance);
            wallHitPosition -= floor(wallHitPosition);
        } else {
            // the wall goes along the y-axis
            wallHitPosition = player.pos.x + (columnDirection.x * wallDistance);
            wallHitPosition -= floor(wallHitPosition);
        }

        // making the height of wall proportional to the screen height, while respecting the FOV
        double wallHeight = RENDER_HEIGHT / wallDistanceNoFishEye * (5.0/3.0);
        //double wallHeight = RENDER_HEIGHT / wallDistanceNoFishEye;
        // Prevent from drawing outside the renderer
        int distanceToHeight;
        wallHeight > RENDER_HEIGHT ? distanceToHeight = RENDER_HEIGHT : distanceToHeight = wallHeight;

        // calculating the highest and lowest pixel of the wall proportional to the screen
        int yTop = (RENDER_HEIGHT / 2) - (distanceToHeight / 2);
        int yBottom = (RENDER_HEIGHT / 2) + (distanceToHeight / 2);

        // loop over all pixel on the column that belong to the wall
        for (int pixel = yTop; pixel <= yBottom; pixel++) {

            // the pixel color needs to be stored in a local variable because it gets modified be the shadow effect
            Pixel color = {0,0,0};
            double pixelOffset = pixel - (RENDER_HEIGHT / 2 - wallHeight / 2);
            vi2D textureCoordinate;
            // flip texture on the x-axis if the wall is on the left or bottom
            if (columnDirection.x < 0 && hitOnAxisX || columnDirection.y > 0 && !hitOnAxisX) {
                textureCoordinate.x = TEXTURE_SIZE - 1 - floor(wallHitPosition * TEXTURE_SIZE);
            } else {
                textureCoordinate.x = floor(wallHitPosition * TEXTURE_SIZE);
            }
            textureCoordinate.y = floor((pixelOffset / wallHeight) * TEXTURE_SIZE);
            getTextureColor(tileTextureID, textureCoordinate, color);

            // add shadow effect to texture if the wall is on the x-axis
            if (hitOnAxisX) applyShadow(color);

            /* modify the corresponding pixel in the buffer
               by modifying the Uint32, where 8 bits stand for one color channel*/
            pixelArray[pixel * RENDER_WIDTH + renderColumn] = color.b + (color.g << 8) + (color.r << 16) + (255 << 24);
        }
    }
}

void applyShadow(Pixel &color) {
    // reduce the color intensity on all 3 color channel
    color.r > SHADOW_STRENGTH ? color.r -= SHADOW_STRENGTH : color.r = 0;
    color.g > SHADOW_STRENGTH ? color.g -= SHADOW_STRENGTH : color.g = 0;
    color.b > SHADOW_STRENGTH ? color.b -= SHADOW_STRENGTH : color.b = 0;
}

int drawMiniMap(SDL_Renderer* renderer, Player &player) {
    // get dimension, then loop over the array
    vi2D mapDimensions = getMatrixDimension();
    int miniMapScale = MINI_MAP_SIZE / mapDimensions.x;

    for (int y = 0; y < mapDimensions.y; y++) {
        for (int x = 0; x < mapDimensions.x; x++) {
            // if on the current cell is a wall, draw a white pixel, otherwise a black one
            getTileInfo(WALL, {x, y}) != 0 ? SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255) : SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

            // create a square that fits the minimap scale
            SDL_FRect rect;
            rect.x = x * miniMapScale;
            rect.y = y * miniMapScale;
            rect.w = miniMapScale;
            rect.h = miniMapScale;
            SDL_RenderFillRect(renderer, &rect);
        }
    }

    // draw line where the player looks at and end the line on the first wall hit
    int tileInfo;
    bool shadowSide;
    double distance = dda(player.pos, player.dir, tileInfo, shadowSide);
    SDL_SetRenderDrawColor(renderer, 100, 100,100,255);
    SDL_RenderLine(renderer, player.pos.x * miniMapScale, player.pos.y * miniMapScale, (player.pos.x + (player.dir.x * distance)) * miniMapScale, (player.pos.y + (player.dir.y * distance)) * miniMapScale);

    // draw player position to the mini map
    SDL_SetRenderDrawColor(renderer, 255,255,255,255);
    SDL_RenderPoint(renderer, player.pos.x * miniMapScale, player.pos.y * miniMapScale);
    return 1;
}
