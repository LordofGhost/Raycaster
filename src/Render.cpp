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

    drawFloor(pixelArray, player);
    drawWall(pixelArray, player);

    // Shift buffer from RAM to the GPU
    SDL_UnlockTexture(buffer);

    return 1;
}

void drawFloor(Uint32* pixelArray, Player &player) {

    for (int renderRow = 0; renderRow < RENDER_HEIGHT/2; renderRow++) {
        // calculate the view angle of camera to the row in the ceiling
        double angle = 50 * (renderRow / double(RENDER_HEIGHT/2)) + 40;

        // the distance from the player to the current row, where 0.5 is the height of the player in the world space
        double distanceToRow = tan(degreeToRad(angle)) * 0.5;

        // the length of the row
        double lengthOfRow = 2 * distanceToRow * tan(degreeToRad(RENDER_FOV/2));

        // the vector points from the left corner of the row all the way to right corner
        vd2D rowDirection;
        rowDirection.x = -player.dir.y * lengthOfRow;
        rowDirection.y = player.dir.x * lengthOfRow;

        vd2D rowPosition;
        rowPosition.x = player.pos.x + player.dir.x * distanceToRow;
        rowPosition.y = player.pos.y + player.dir.y * distanceToRow;

        // move the position to the left corner of the row
        rowPosition.x += -(rowDirection.x / 2);
        rowPosition.y += -(rowDirection.y / 2);

        for (int pixel = 0; pixel < RENDER_WIDTH; pixel++) {
            Pixel color = {0,0,0};

            // this scaler is a value between 0 and 1 which is used to apply the vector of the row
            double pixelScaler = pixel / double(RENDER_WIDTH);

            vd2D pixelPositionOnTheRow;
            pixelPositionOnTheRow.x = rowPosition.x + rowDirection.x * pixelScaler;
            pixelPositionOnTheRow.y = rowPosition.y + rowDirection.y * pixelScaler;

            vi2D textureCoordinate;
            textureCoordinate.x = (pixelPositionOnTheRow.x - floor(pixelPositionOnTheRow.x)) * TEXTURE_SIZE;
            textureCoordinate.y = (pixelPositionOnTheRow.y - floor(pixelPositionOnTheRow.y)) * TEXTURE_SIZE;

            int tileTextureID = getTileInfo(FLOOR,{(int)pixelPositionOnTheRow.x, (int)pixelPositionOnTheRow.y});

            getTextureColor(tileTextureID, textureCoordinate, color);

            pixelArray[pixel + RENDER_WIDTH * RENDER_HEIGHT - (RENDER_WIDTH * renderRow)] = color.r + (color.g << 8) + (color.b << 16) + (255 << 24);
        }
    }
}

void drawCeiling(Uint32* pixelArray, Player &player) {

    for (int renderRow = 0; renderRow < RENDER_HEIGHT/2; renderRow++) {

        /* the row is a line in front of the player that is orthogonal to the player.dir,
         * the length of the line depends on the distance and FOV */
        double rowScale = double(RENDER_HEIGHT/2) / (double(RENDER_HEIGHT/2) / renderRow);
        vd2D rowPosition;
        // the vector is scaled to go from the middle all the way to the right of the screen
        vd2D rowDirection = {player.dir.y * rowScale, -player.dir.x * rowScale};
        rowPosition.x = player.pos.x + player.dir.x * (renderRow/double(RENDER_HEIGHT/2));
        rowPosition.y = player.pos.y + player.dir.y * (renderRow/double(RENDER_HEIGHT/2));

        for (int pixel = 0; pixel < RENDER_WIDTH; pixel++) {
            Pixel color = {255,255,255};
            // number between -1 and 1, representing a position the row
            double pixelToScale = (pixel - double(RENDER_WIDTH/2)) / double(RENDER_WIDTH/2);
            vd2D textureCoordinate;
            textureCoordinate.x = rowPosition.x + rowDirection.x * pixelToScale;
            textureCoordinate.y = rowPosition.y + rowDirection.y * pixelToScale;
            //int tileTextureID = getTileInfo(FLOOR,{(int)textureCoordinate.x - 1, (int)textureCoordinate.y - 1});
            //getTextureColor(1, textureCoordinate.x, textureCoordinate.y, color);

            pixelArray[pixel + RENDER_WIDTH * renderRow] = color.b + (color.g << 8) + (color.r << 16) + (255 << 24) ;
        }
    }
}

void drawWall(Uint32* pixelArray, Player &player) {

    for (int renderColumn = 0; renderColumn < RENDER_WIDTH; renderColumn++) {

        // calculating the pixel column specific units
        double columnStepSize = (double)RENDER_FOV / (double)RENDER_WIDTH;
        double columnAngle = (columnStepSize * renderColumn) - (RENDER_FOV / 2);
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
        double wallHeight = RENDER_HEIGHT / wallDistanceNoFishEye * 0.5;
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
            pixelArray[pixel * RENDER_WIDTH + renderColumn] = color.r + (color.g << 8) + (color.b << 16) + (255 << 24) ;
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
