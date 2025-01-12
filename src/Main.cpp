#define SDL_MAIN_USE_CALLBACKS 1  // use the callbacks instead of main()
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h> // needed for callbacks
#include <SDL3/SDL_events.h>
#include <math.h>
#include "Main.h"
#include "Map.h"
#include "Render.h"

static int windowHeight = RENDER_HEIGHT;
static int windowWidth = RENDER_WIDTH;
static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;
static Player player;

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

    if (event->type == SDL_EVENT_KEY_DOWN) {
        double oldDirX = player.dir.x;
        switch (event->key.key) {
            case W:
                player.pos.y += player.dir.y * MOVE_DISTANCE;
                player.pos.x += player.dir.x * MOVE_DISTANCE;
                break;
            case A:
                player.dir.x = (cos(-ROTATE_ANGLE) * player.dir.x) + (-sin(-ROTATE_ANGLE) * player.dir.y);
                player.dir.y = (sin(-ROTATE_ANGLE) * oldDirX) + (cos(-ROTATE_ANGLE) * player.dir.y);
                break;
            case S:
                player.pos.y -= player.dir.y * MOVE_DISTANCE;
                player.pos.x -= player.dir.x * MOVE_DISTANCE;
                break;
            case D:
                player.dir.x = (cos(ROTATE_ANGLE) * player.dir.x) + (-sin(ROTATE_ANGLE) * player.dir.y);
                player.dir.y = (sin(ROTATE_ANGLE) * oldDirX) + (cos(ROTATE_ANGLE) * player.dir.y);
                break;
        }
    }

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

    draw3dSpace(renderer, player);
    drawMiniMap(renderer, player);

    /* put the newly-cleared rendering on the screen. */
    SDL_RenderPresent(renderer);

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {}