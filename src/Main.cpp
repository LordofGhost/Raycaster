#define SDL_MAIN_USE_CALLBACKS 1  // use the callbacks instead of main()
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h> // needed for callbacks
#include <SDL3/SDL_events.h>
#include <iostream>

#include "Main.h"
#include "DDA.h"
#include "Map.h"
#include "Render.h"

static int windowHeight = RENDER_HEIGHT;
static int windowWidth = RENDER_WIDTH;
static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;
static Player player;
static KeyState keyState;
static Uint32 lastFrameTime = 0;
static int fpsCounter = 0;
static Uint64 timeLastPrintFPSCounter = 0;

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    SDL_SetAppMetadata("Raycaster", "0.1", "com.lordofghost.raycaster");

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!SDL_CreateWindowAndRenderer("Raycaster", windowWidth, windowHeight, 0, &window, &renderer)) {
        SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    SDL_SetWindowResizable(window, true);

    setStartingPoint(player);

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{

    if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;  // end the program, reporting success to the OS
    }

    // control movement
    if (event->type == SDL_EVENT_KEY_DOWN || event->type == SDL_EVENT_KEY_UP) {
        switch (event->key.key) {
            case W:
                event->type == SDL_EVENT_KEY_DOWN ? keyState.w = true : keyState.w = false;
                break;
            case A:
                event->type == SDL_EVENT_KEY_DOWN ? keyState.a = true : keyState.a = false;
                break;
            case S:
                event->type == SDL_EVENT_KEY_DOWN ? keyState.s = true : keyState.s = false;
                break;
            case D:
                event->type == SDL_EVENT_KEY_DOWN ? keyState.d = true : keyState.d = false;
                break;
        }
    }

    // scale the window on resize
    if (event->type == SDL_EVENT_WINDOW_RESIZED) {
        windowWidth = event->window.data1;
        windowHeight = event->window.data2;
        SDL_SetRenderScale(renderer,windowWidth / RENDER_WIDTH, windowHeight / RENDER_HEIGHT);
    }

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate)
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    Uint64 timeSinceStart = SDL_GetTicks();
    Uint64 deltaTime = timeSinceStart - lastFrameTime;
    lastFrameTime = timeSinceStart;

    fpsCounter++;
    if ((timeSinceStart - timeLastPrintFPSCounter) >= 1000) {
        std::cout << "FPS: " << fpsCounter << std::endl;
        timeLastPrintFPSCounter = timeSinceStart;
        fpsCounter = 0;
    }

    if (keyState.w) {
        if (getTileInfo({(int) (player.pos.x + PLAYER_COLLISION_RADIUS * (player.dir.x >= 0 ? 1 : -1)),(int) player.pos.y}) == 0) {
            player.pos.x = player.pos.x + player.dir.x * MOVE_DISTANCE * deltaTime;
        }
        if (getTileInfo({(int) player.pos.x,(int) (player.pos.y + PLAYER_COLLISION_RADIUS * (player.dir.y >= 0 ? 1 : -1))}) == 0) {
            player.pos.y = player.pos.y + player.dir.y * MOVE_DISTANCE * deltaTime;
        }
    }
    if (keyState.a) {
        // rotate camera to the right
        player.dir = rotateVector(player.dir, -ROTATE_ANGLE * deltaTime);
    }
    if (keyState.s) {
        if (getTileInfo({(int) (player.pos.x - PLAYER_COLLISION_RADIUS * (player.dir.x >= 0 ? 1 : -1)),(int) player.pos.y}) == 0) {
            player.pos.x = player.pos.x - player.dir.x * MOVE_DISTANCE * deltaTime;
        }
        if (getTileInfo({(int) player.pos.x,(int) (player.pos.y - PLAYER_COLLISION_RADIUS * (player.dir.y >= 0 ? 1 : -1))}) == 0) {
            player.pos.y = player.pos.y - player.dir.y * MOVE_DISTANCE * deltaTime;
        }
    }
    if (keyState.d) {
        // rotate camera to the left;
        player.dir = rotateVector(player.dir, ROTATE_ANGLE * deltaTime);
    }

    draw3dSpace(renderer, player);
    drawMiniMap(renderer, player);

    /* put the newly-cleared rendering on the screen. */
    SDL_RenderPresent(renderer);

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {}