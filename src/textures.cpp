#include "textures.h"

#include "resources.h"

#include <map>
#include <string.h>

std::map<std::string, Texture> loadedTextures;

bool GetTexture(std::string path, Texture& t) {
	if(loadedTextures.count(path)) {
		t = loadedTextures[path];
		return true;
	};
	signed long long fileLength = -1;
	char* fileData = ReadPHYSFSFileLength(path, fileLength);
	if(fileData == NULL) {
		return false;
	}
	Image img = LoadImageFromMemory(GetFileExtension(path.c_str()), reinterpret_cast<unsigned char*>(fileData), fileLength);
	if(img.data == NULL) {
		return false;
	}
	free(fileData);
	Texture tex = LoadTextureFromImage(img);
	UnloadImage(img);
	loadedTextures.insert_or_assign(path, tex);
	t = tex;
	return true;
}

void UnloadTextures() {
	for(std::pair<std::string, Texture> v : loadedTextures) {
		UnloadTexture(v.second);
	}
}