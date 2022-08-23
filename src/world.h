#pragma once

#include <raylib.h>

#include <vector>

typedef struct WorldObject {
	Mesh mesh;
	Material material;
	Vector3 position;
	Vector3 rotation;
	float scale;
} WorldObject;

typedef struct World {
	std::vector<WorldObject> objects;
} World;

void DrawWorld(World* w);
