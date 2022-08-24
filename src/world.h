#pragma once

#include <raylib.h>

#include <vector>
#include <string>

class WorldObject {
public:
	int id;
	std::string name;
	std::string filepath;
	Mesh mesh;
	Material material;
	Vector3 position;
	Vector3 rotation;
	float scale;
};

class World {
private:
	int nextObjectId = 1;
public:
	Shader alphaDiscardShader;
	std::vector<WorldObject> objects;
	World();
	void Unload();
	int AddObject(std::string filepath);
	void Draw();
};
