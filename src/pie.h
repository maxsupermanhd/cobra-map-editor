#pragma once

#include <raylib.h>

#include <string>

struct PIEpolygon {
	int flags = 0;
	int pcount = 0;
	int porder[3];
	float texcoords[6];
};

struct PIEshadowpolygon {
	int flags = 0;
	int pcount = 0;
	int porder[3];
};

struct PIEanimframe {
	int frame = 0;
	Vector3 pos;
	Vector3 rot;
	Vector3 scale;
};

class PIElevel {
public:
	Vector3* points = nullptr;
	int pointscount = 0;
	Vector3* normals = nullptr;
	int normalscount = 0;
	PIEpolygon* polygons = nullptr;
	int polygonscount = 0;
	Vector3* connectors = nullptr;
	int connectorscount = 0;
	PIEanimframe* anim = nullptr;
	int animtime = 0;
	int animcycles = 0;
	int animframes = 0;
	Vector3* shadowpoints = nullptr;
	int shadowpointscount = 0;
	PIEshadowpolygon* shadowpolygons = nullptr;
	int shadowpolygonscount = 0;
	void InitAtZero();
	~PIElevel();
};

class PIEmodel {
public:
	int ver = 0;
	int type;
	bool interpolate = true;
	std::string texturename = "";
	int tsizew = 0, tsizeh = 0;
	std::string normalname = "";
	std::string specularname = "";
	std::string events[3] = {"", "", ""};
	PIElevel* levels = nullptr;
	// starts from 0! level 1 becomes 0 etc!
	int levelscount = 0;
	PIEmodel();
	bool ReadPIE(std::string path);
	~PIEmodel();
};

void FreeModels();
bool LoadFromPIE(std::string filepath, Mesh* ret);
