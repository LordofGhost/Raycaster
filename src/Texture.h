
#ifndef TEXTURE_H
#define TEXTURE_H

#include <filesystem>
#include <SDL3/SDL_stdinc.h>
#include "Main.h"

struct Pixel {
    Uint8 r;
    Uint8 g;
    Uint8 b;
};

int getTextureColor(int textureID, int x, int y, Pixel &color);
int loadPngTextures(const std::filesystem::path& directory);
void getRGBValuesFromPNG(const std::string& filePath);

#endif //TEXTURE_H
