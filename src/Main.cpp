#define SDL_MAIN_USE_CALLBACKS 1  // use the callbacks instead of main()
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h> // needed for callbacks
#include <SDL3/SDL_events.h>
#include <iostream>
#include <math.h>

#define MOVE_DISTANCE 0.5
#define PI 3.14159

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;

enum keyCodes {
    W = 119,
    A = 97,
    S = 115,
    D = 100,
};

struct Player {
    double positionX;
    double positionY;
    double angle;
} player;

struct v2D {
    double x;
    double y;
};

int map[8][8] {
    { 0, 0, 0, 0, 1, 1, 1, 1 },
    { 0, 0, 0, 0, 0, 0, 0, 1 },
    { 0, 0, 0, 1, 1, 1, 0, 1 },
    { 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 1, 1, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 1, 1, 1, 1, 1, 1, 1 },
};

void drawMap() {
    SDL_SetRenderDrawColor(renderer, 255,255,255,255);
    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            if (map[y][x]) SDL_RenderPoint(renderer, x, y);
        }
    }
}

bool checkForWall(int x, int y) {
    return map[y][x] == 1;
}

void drawPlayer() {
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderLine(renderer, player.positionX, -player.positionY, player.positionX + (std::cos(player.angle) * 5), -(player.positionY + (std::sin(player.angle) * 5)));
    //SDL_RenderPoint(renderer, player.positionX, -player.positionY);
}

double getWallDistance(double x, double y, double angel, int remainChecks) {
    v2D currentCell = { std::floor(x), std::floor(y)};
    v2D nextCell = {};
    double distance = 0;
    // the normalized coordinate in relation to current cell
    double xNorm = x - currentCell.x;
    double yNorm = y - currentCell.y;

    /*
     * calculates the length of the hypotenuse, from the triangle that sits between the normalized coordinate the
     * border of cell, considering the direction the angle points at
     */
    double xHypo;
    if (std::cos(angel) != 0)
        if (std::cos(angel) > 0)
            xHypo = std::abs((1 - xNorm) / std::cos(angel));
        else
            xHypo = std::abs(xNorm / std::cos(angel));
    else
        xHypo = std::numeric_limits<double>::infinity();

    double yHypo;
    if (std::sin(angel) != 0)
        if (std::sin(angel) > 0)
            yHypo = std::abs(yNorm / std::sin(angel));
        else
            yHypo = std::abs((1 - yNorm) / std::sin(angel));
    else
        yHypo = std::numeric_limits<double>::infinity();

    // move the border of the cell where the hypo is lower
    if (xHypo < yHypo) {
        x = (cos(angel) >= 0) ? std::floor(x) + 1 : std::floor(x);
        y += std::sin(angel) * xHypo;
        distance += xHypo;
        nextCell.x = (cos(angel) >= 0) ? currentCell.x + 1 : currentCell.x - 1;
    } else {
        x += std::cos(angel) * yHypo;
        y = (sin(angel) >= 0) ? std::floor(y) : std::floor(y) + 1;
        distance += yHypo;
        nextCell.y = (sin(angel) >= 0) ? currentCell.y+1 : currentCell.y-1;
    }

    remainChecks--;
    if (!checkForWall(nextCell.x, nextCell.y) && remainChecks) return getWallDistance(x, y, angel, remainChecks) + distance;

    return distance;
}

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    SDL_SetAppMetadata("Raycaster", "0.1", "com.lordofghost.raycaster");

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!SDL_CreateWindowAndRenderer("Raycaster", 640, 480, 0, &window, &renderer)) {
        SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    SDL_SetRenderScale(renderer, 10, 10);
    player.positionX = 0,player.positionY = 0, player.angle = 0;

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{

    if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;  // end the program, reporting success to the OS
    }

    if (event->type == SDL_EVENT_KEY_DOWN) {
        switch (event->key.key) {
            case W:
                player.positionX += std::cos(player.angle) * MOVE_DISTANCE;
                player.positionY += std::sin(player.angle) * MOVE_DISTANCE;
                break;
            case A:
                player.angle += 0.1;
                break;
            case S:
                player.positionX -= std::cos(player.angle) * MOVE_DISTANCE;
                player.positionY -= std::sin(player.angle) * MOVE_DISTANCE;
                break;
            case D:
                player.angle -= 0.1;
                break;
        }
        std::cout << "Distance: " << getWallDistance(player.positionX, player.positionY, player.angle, 16) << std::endl;
    }

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate)
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    drawPlayer();
    drawMap();

    /* put the newly-cleared rendering on the screen. */
    SDL_RenderPresent(renderer);

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {}