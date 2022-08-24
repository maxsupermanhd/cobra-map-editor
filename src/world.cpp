#include "world.h"

#include <raylib.h>
#include <raymath.h>

#include "pie.h"
#include "textures.h"

#include <string>

const char* alphaDiscardShaderSrc = R"SHADERCODE(
#version 330
in vec2 fragTexCoord;
in vec4 fragColor;
out vec4 finalColor;
uniform sampler2D texture0;
uniform vec4 colDiffuse;

void main()
{
	vec4 texelColor = texture(texture0, fragTexCoord); 
	if(texelColor.a <= 0.0) {
		discard;
	}
	finalColor = texelColor*colDiffuse*fragColor;
}
)SHADERCODE";

void World::Draw() {
	for(WorldObject o : this->objects) {
		DrawMesh(o.mesh, o.material, MatrixIdentity());
	}
}

World::World() {
	this->alphaDiscardShader = LoadShaderFromMemory(NULL, alphaDiscardShaderSrc);
}

void World::Unload() {
	UnloadShader(this->alphaDiscardShader);
}

int World::AddObject(std::string filepath) {
	WorldObject o;
	PIEmodel* model = GetModel(filepath);
	if(model == NULL) {
		TraceLog(LOG_ERROR, "Failed to load PIE model from %s", filepath.c_str());
		return false;
	}
	Mesh mesh = {0};
	if(!LoadMeshFromPIE(model, &mesh)) {
		TraceLog(LOG_ERROR, "Failed to load mesh from pie, file %s", filepath.c_str());
		return false;
	}
	Material mat = LoadMaterialDefault();
	mat.maps[MATERIAL_MAP_DIFFUSE].texture = GetTexture("./data/page-7-barbarians-arizona.png");
	for(unsigned int i = 0; i < mesh.vertexCount; i++) {
		mesh.texcoords[i*2+0] /= mat.maps[MATERIAL_MAP_DIFFUSE].texture.height;
		mesh.texcoords[i*2+1] /= mat.maps[MATERIAL_MAP_DIFFUSE].texture.width;
	}
	UploadMesh(&mesh, false);
	mat.shader = alphaDiscardShader;
	o.mesh = mesh;
	o.material = mat;
	o.scale = 1.0f;
	o.id = this->nextObjectId;
	this->nextObjectId++;
	this->objects.push_back(o);
	return true;
}