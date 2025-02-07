#include "Texture.h"
#include <filesystem>
#include <iostream>
#include <string>
#include <SDL3_image/SDL_image.h>

/*  the textures are stored in the data segment, so the memory is allocated at compile time
    array structure: ID, column, row, pixel colors */
static std::array<std::array<std::array<Pixel, TEXTURE_SIZE>, TEXTURE_SIZE>, TEXTURE_COUNT> textures;

int loadPngTextures(const std::filesystem::path& directory) {
    int loadedTextures = 0;

    // check if the path is correct
    if (std::filesystem::exists(directory) && std::filesystem::is_directory(directory)) {
        // loop over all files in the directory
        for (const auto& entry : std::filesystem::directory_iterator(directory)) {
            // load the file if it's a png
            if (entry.is_regular_file() && entry.path().extension() == ".png") {
                getRGBValuesFromPNG(entry.path());
                loadedTextures++;
            }
        }

        if (!loadedTextures) {
            if (DEBUG) std::cerr << "loadPngTextures: no textures could be loaded" << std::endl;
        } else {
            if (DEBUG) std::cout << "loadPngTextures: " << loadedTextures << " textures have been loaded" << std::endl;
        }
    } else {
        std::cerr << "loadPngTextures: no directory found" << std::endl;
        return -1;
    }

    return 1;
}

void getRGBValuesFromPNG(const std::string& filePath) {
    // get the ID of the texture by the file name
    std::string filePathString = filePath.c_str();
    int textureSlot = filePathString[filePath.length() - 5] - 48 - 1;

    // create memory and load the image in
    SDL_Surface* surface = IMG_Load(filePath.c_str());
    if (!surface) {
        if (DEBUG) std::cerr << "getRGBValuesFromPNG: file could not be loaded" << std::endl;
        return;
    }

    // move the memory of the image to the ram
    SDL_LockSurface(surface);

    Uint32* pixels = (Uint32*)surface->pixels;
    // check if the texture has the right resolution
    if (surface->w != TEXTURE_SIZE || surface->h != TEXTURE_SIZE) {
        if (DEBUG) std::cerr << "getRGBValuesFromPNG: file with wrong resolution at " << filePathString << std::endl;
        return;
    }

    // loop over every pixel of the surface
    for (int y = 0; y < TEXTURE_SIZE; ++y) {
        for (int x = 0; x < TEXTURE_SIZE; ++x) {
            // take the stored data for the current pixel from the surface
            Uint32 pixel = pixels[(y * TEXTURE_SIZE) + x];
            Uint8 r, g, b;
            // get the format the data for pixel
            const SDL_PixelFormatDetails* format = SDL_GetPixelFormatDetails(SDL_PIXELFORMAT_RGB24);
            // deconstruct the pixel data
            SDL_GetRGB(pixel, format, nullptr, &r, &g, &b);
            // set the rgb value in the texture array
            textures[textureSlot][x][y].r = r;
            textures[textureSlot][x][y].g = g;
            textures[textureSlot][x][y].b = b;
        }
    }

    // move it out of ram
    SDL_UnlockSurface(surface);
    // delete it from the gpu
    SDL_DestroySurface(surface);
}

void getTextureColor(const int textureID, vi2D position, Pixel &color) {
    if (textureID == 0) return;
    if (position.x > TEXTURE_SIZE || position.y > TEXTURE_SIZE) {
        if (DEBUG) std::cerr << "getTextureColor: trying to get color value outside of texture" << std::endl;
        return;
    }
    if (textureID > TEXTURE_COUNT - 1) {
        if (DEBUG) std::cerr << "getTextureColor: trying to read from not existing texture" << std::endl;
        return;
    }
    // get the according rgb value of the pixel
    color.r = textures[textureID-1][position.x][position.y].r;
    color.g = textures[textureID-1][position.x][position.y].g;
    color.b = textures[textureID-1][position.x][position.y].b;
}