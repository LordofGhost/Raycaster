#include "Texture.h"
#include <filesystem>
#include <iostream>
#include <string>
#include <SDL3_image/SDL_image.h>

// array structure: ID, column, row, pixel colors
std::array<std::array<std::array<Pixel, TEXTURE_SIZE>, TEXTURE_SIZE>, 10> textures;

int loadPngTextures(const std::filesystem::path& directory) {

    // find all .png files in the given directory
    if (std::filesystem::exists(directory) && std::filesystem::is_directory(directory)) {
        for (const auto& entry : std::filesystem::directory_iterator(directory)) {
            if (entry.is_regular_file() && entry.path().extension() == ".png") {
                getRGBValuesFromPNG(entry.path());
            }
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
    int textureSlot = filePathString[filePath.length() - 5] - 48;

    // create memory and load the image in
    SDL_Surface* surface = IMG_Load(filePath.c_str());
    if (!surface) {
        std::cerr << "getRGBValuesFromPNG: file could not be loaded" << std::endl;
        return;
    }

    // move the memory of the image to the ram
    SDL_LockSurface(surface);

    // TODO move this to array creation, in oder to move textures to the heap instead of data segment
    Uint32* pixels = (Uint32*)surface->pixels;
    int width = surface->w;
    int height = surface->h;

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            Uint32 pixel = pixels[(y * width) + x];
            Uint8 r, g, b;
            const SDL_PixelFormatDetails* format = SDL_GetPixelFormatDetails(SDL_PIXELFORMAT_RGB24);
            SDL_GetRGB(pixel, format, nullptr, &r, &g, &b);
            textures[textureSlot][x][y].r = r;
            textures[textureSlot][x][y].g = g;
            textures[textureSlot][x][y].b = b;
        }
    }

    SDL_UnlockSurface(surface);
    SDL_DestroySurface(surface);
}

int getTextureColor(const int textureID, const int x, const int y, int* pixelColor) {
    pixelColor[0] = textures[textureID-1][x][y].r;
    pixelColor[1] = textures[textureID-1][x][y].g;
    pixelColor[2] = textures[textureID-1][x][y].b;
    return 1;
}

int getTextureDimensions(int textureID) {
    return TEXTURE_SIZE;
}
