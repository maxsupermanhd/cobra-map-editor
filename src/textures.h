#pragma once

#include <raylib.h>

#include <string>

bool GetTexture(std::string path, Texture& t);
void UnloadTextures();
