#include "textures.h"

#include <map>
#include <string>

std::map<std::string, Texture> loadedTextures;

Texture GetTexture(const char* path) {
	Texture tex = LoadTexture(path);
	loadedTextures.insert_or_assign(path, tex);
	return tex;
}

void UnloadTextures() {
	for(std::pair<std::string, Texture> v : loadedTextures) {
		UnloadTexture(v.second);
	}
}