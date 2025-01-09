#define SDL_MAIN_USE_CALLBACKS 1  // use the callbacks instead of main()
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h> // needed for callbacks
#include <SDL3/SDL_events.h>
#include <iostream>
#include <math.h>

#define RENDER_HEIGHT 480
#define RENDER_WIDTH 640
#define RENDER_FOV 60
#define MOVE_DISTANCE 0.2

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

bool checkForWall(int x, int y) {
    return map[y][x] == 1;
}

double getWallDistance(double x, double y, double angle, int remainChecks) {
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
    if (std::cos(angle) != 0)
        if (std::cos(angle) > 0)
            xHypo = std::abs((1 - xNorm) / std::cos(angle));
        else
            xHypo = std::abs(xNorm / std::cos(angle));
    else
        xHypo = std::numeric_limits<double>::infinity();

    double yHypo;
    if (std::sin(angle) != 0)
        if (std::sin(angle) > 0)
            yHypo = std::abs(yNorm / std::sin(angle));
        else
            yHypo = std::abs((1 - yNorm) / std::sin(angle));
    else
        yHypo = std::numeric_limits<double>::infinity();

    // move the border of the cell where the hypo is lower
    if (xHypo < yHypo) {
        x = (cos(angle) >= 0) ? std::floor(x) + 1 : std::floor(x);
        y += std::sin(angle) * xHypo;
        distance += xHypo;
        nextCell.x = (cos(angle) >= 0) ? currentCell.x + 1 : currentCell.x - 1;
    } else {
        x += std::cos(angle) * yHypo;
        y = (sin(angle) >= 0) ? std::floor(y) : std::floor(y) + 1;
        distance += yHypo;
        nextCell.y = (sin(angle) >= 0) ? currentCell.y+1 : currentCell.y-1;
    }

    remainChecks--;
    if (!checkForWall(nextCell.x, nextCell.y) && remainChecks) return getWallDistance(x, y, angle, remainChecks) + distance;

    return distance;
}

int draw3dSpace(Player player) {

    SDL_SetRenderDrawColor(renderer, 255,255,255,255);
    for (int renderColumn = 0; renderColumn < RENDER_WIDTH; renderColumn++) {
        double angle = (player.angle - (RENDER_FOV * (M_PI / 180.0)) / 2) + (RENDER_FOV * (M_PI / 180.0) / RENDER_WIDTH) * renderColumn;
        double distance = getWallDistance(player.positionX, player.positionY, angle, 16);

        int distanceToHeight = (10 / distance) * 10;
        int yTop = (RENDER_HEIGHT / 2) - (distanceToHeight / 2);
        int yBottom = (RENDER_HEIGHT / 2) + (distanceToHeight / 2);
        SDL_RenderLine(renderer, renderColumn, yTop, renderColumn, yBottom);
    }
    return 1;
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

    //SDL_SetRenderScale(renderer, 10, 10);
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
                player.angle -= 0.1;
                break;
            case S:
                player.positionX -= std::cos(player.angle) * MOVE_DISTANCE;
                player.positionY -= std::sin(player.angle) * MOVE_DISTANCE;
                break;
            case D:
                player.angle += 0.1;
                break;
        }
    }

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate)
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    draw3dSpace(player);

    /* put the newly-cleared rendering on the screen. */
    SDL_RenderPresent(renderer);

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {}