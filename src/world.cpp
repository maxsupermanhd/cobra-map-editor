#include "world.h"

#include <raymath.h>

#include <string>

void DrawWorld(World* w) {
	for(WorldObject o : w->objects) {
		DrawMesh(o.mesh, o.material, MatrixIdentity());
	}
}

WorldObject LoadObject(std::string id, Vector3 pos, Vector3 rot, float scale) {
	WorldObject o = {0};
	return o;
}