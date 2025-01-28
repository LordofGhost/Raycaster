#define SDL_MAIN_USE_CALLBACKS 1  // use the callbacks instead of main()
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h> // needed for callbacks
#include <SDL3/SDL_events.h>
#include <iostream>

#include "Main.h"
#include "DDA.h"
#include "Map.h"
#include "Render.h"
#include "Texture.h"

static int windowHeight = RENDER_HEIGHT;
static int windowWidth = RENDER_WIDTH;
static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;
static SDL_Texture *buffer = NULL;
static SDL_FRect bufferTargetSize = {0, 0, RENDER_WIDTH, RENDER_HEIGHT};
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

    // load the color values in the data segment
    loadPngTextures("../res/textures");

    // create buffer where every frame is written to
    buffer = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, RENDER_WIDTH, RENDER_HEIGHT);
    SDL_SetTextureScaleMode(buffer, SDL_SCALEMODE_NEAREST);

    // enable window resizing and full screen mode
    SDL_SetWindowResizable(window, true);

    // set the player starting position and rotation
    setStartingPoint(player);

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{

    if (event->type == SDL_EVENT_QUIT) {
        // end the program, reporting success to the OS
        return SDL_APP_SUCCESS;
    }

    // using key states to be able to move in more than one direction at a time and make it smooth
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
        // get window size from event object
        windowWidth = event->window.data1;
        windowHeight = event->window.data2;

        // calculate the needed scaling to fill the screen, while keeping the render resolution and the set it
        SDL_SetRenderScale(renderer,windowWidth / (float)RENDER_WIDTH, windowHeight / (float)RENDER_HEIGHT);
    }

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate)
{
    // clear the frame before drawing the new frame
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    Uint64 timeSinceStart = SDL_GetTicks();
    // deltaTime is the time passed since the last frame
    Uint64 deltaTime = timeSinceStart - lastFrameTime;
    lastFrameTime = timeSinceStart;

    fpsCounter++;
    // the fpsCounter if one second is passed since the last print of it
    if ((timeSinceStart - timeLastPrintFPSCounter) >= 1000) {
        std::cout << "FPS: " << fpsCounter << std::endl;
        timeLastPrintFPSCounter = timeSinceStart;
        fpsCounter = 0;
    }

    if (keyState.w) {
        /*  check if the player is allowed to move on the x-axis
            by taking the player position and displacing it by the collision radius
            the displacement factor has to be positive or negative depending on the move direction */
        if (getTileInfo(WALL, {(int) (player.pos.x + PLAYER_COLLISION_RADIUS * (player.dir.x >= 0 ? 1 : -1)),(int) player.pos.y}) == 0) {
            player.pos.x = player.pos.x + player.dir.x * MOVE_DISTANCE * deltaTime;
        }
        // check if the player id allowed to move on the y-axis
        if (getTileInfo(WALL, {(int) player.pos.x,(int) (player.pos.y + PLAYER_COLLISION_RADIUS * (player.dir.y >= 0 ? 1 : -1))}) == 0) {
            player.pos.y = player.pos.y + player.dir.y * MOVE_DISTANCE * deltaTime;
        }
    }
    if (keyState.a) {
        // rotate camera to the right
        player.dir = rotateVector(player.dir, -ROTATE_ANGLE * deltaTime);
    }
    if (keyState.s) {
        if (getTileInfo(WALL, {(int) (player.pos.x - PLAYER_COLLISION_RADIUS * (player.dir.x >= 0 ? 1 : -1)),(int) player.pos.y}) == 0) {
            player.pos.x = player.pos.x - player.dir.x * MOVE_DISTANCE * deltaTime;
        }
        if (getTileInfo(WALL, {(int) player.pos.x,(int) (player.pos.y - PLAYER_COLLISION_RADIUS * (player.dir.y >= 0 ? 1 : -1))}) == 0) {
            player.pos.y = player.pos.y - player.dir.y * MOVE_DISTANCE * deltaTime;
        }
    }
    if (keyState.d) {
        // rotate camera to the left;
        player.dir = rotateVector(player.dir, ROTATE_ANGLE * deltaTime);
    }

    // call the render function
    draw3dSpace(buffer, player);
    // put the buffer data to the renderer
    SDL_RenderTexture(renderer, buffer, NULL, &bufferTargetSize);

    // render the mini map over the 3d space
    drawMiniMap(renderer, player);

    // display the image to the screen
    SDL_RenderPresent(renderer);

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {}