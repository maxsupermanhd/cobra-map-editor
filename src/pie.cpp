#include "pie.h"

#include <raylib.h>

#include "misc.h"
#include "resources.h"

#include <string>
#include <cstring>
#include <map>
#include <errno.h>
#include <string.h>

void PIElevel::InitAtZero() {
	this->points = nullptr;
	this->pointscount = 0;
	this->normals = nullptr;
	this->normalscount = 0;
	this->polygons = nullptr;
	this->polygonscount = 0;
	this->connectors = nullptr;
	this->connectorscount = 0;
	this->anim = nullptr;
	this->animtime = 0;
	this->animcycles = 0;
	this->animframes = 0;
	this->shadowpoints = nullptr;
	this->shadowpointscount = 0;
	this->shadowpolygons = nullptr;
	this->shadowpolygonscount = 0;
}

PIElevel::~PIElevel() {
	if(this->points) {
		free(this->points);
	}
	if(this->normals) {
		free(this->normals);
	}
	if(this->connectors) {
		free(this->connectors);
	}
	if(this->anim) {
		free(this->anim);
	}
	if(this->shadowpoints) {
		free(this->shadowpoints);
	}
	if(this->shadowpolygons) {
		free(this->shadowpolygons);
	}
}

PIEmodel::~PIEmodel() {
	if(this->levels) {
		for(int i = 0; i < this->levelscount; i++) {
			this->levels[i].~PIElevel();
		}
		free(this->levels);
	}
}

PIEmodel::PIEmodel() {
}

bool PIEmodel::ReadPIE(char* content) {
	int ret = sscanf(content, "PIE %d\nTYPE %d\n", &this->ver, &this->type);
	if(ret != 2) {
		TraceLog(LOG_ERROR, "Failed to parse first 2 lines of PIE, ret %d", ret);
		return false;
	}

	char* str = strchr(content, '\n');
	if(str == NULL) {
		return false;
	}
	str++;

	int snum = 3;
	int nowlevel = -1;
	while(1) {
		str = strchr(str, '\n');
		if(str == NULL) {
			return true;
		}
		str++;
		if(!strncmpl(str, "INTERPOLATE")) {
			int v;
			int r = sscanf(str, "INTERPOLATE %d", &v);
			if(r != 1) {
				TraceLog(LOG_ERROR, "PIE READ INTERPOLATE line %d ret %d", snum, r);
				return false;
			}
			this->interpolate = v;
		} else if(!strncmpl(str, "TEXTURE")) {
			char* texname; // technically we must free that pointer
			int r = sscanf(str, "TEXTURE %*d %ms %d %d", &texname, &this->tsizeh, &this->tsizew);
			if(r != 3) { // but what happens if that pointer never actually allocated
				TraceLog(LOG_ERROR, "PIE READ TEXTURE line %d ret %d", snum, r);
				// free(texname); double free?
				return false;
			}
			this->texturename = std::string(texname);
			free(texname);
		} else if(!strncmpl(str, "NORMALMAP")) {
			char* texname;
			int r = sscanf(str, "NORMALMAP %*d %ms", &texname);
			if(r != 1) {
				TraceLog(LOG_ERROR, "PIE READ NORMALMAP line %d ret %d", snum, r);
				return false;
			}
			this->normalname = std::string(texname);
			free(texname);
		} else if(!strncmpl(str, "SPECULARMAP")) {
			char* texname;
			int r = sscanf(str, "SPECULARMAP %*d %ms", &texname);
			if(r != 1) {
				TraceLog(LOG_ERROR, "PIE READ SPECULARMAP line %d ret %d", snum, r);
				return false;
			}
			this->specularname = std::string(texname);
			free(texname);
		} else if(!strncmpl(str, "EVENT")) {
			int t;
			char* file;
			int r = sscanf(str, "EVENT %d %ms", &t, &file);
			if(r != 2) {
				TraceLog(LOG_ERROR, "PIE READ EVENT line %d ret %d", snum, r);
				return false;
			}
			if(t < 0 || t > 3) {
				TraceLog(LOG_ERROR, "Not supported PIE [%s] EVENT %d line %d ret %d", t, snum, r);
			} else {
				this->events[t] = std::string(file);
			}
			free(file);
		} else if(!strncmpl(str, "LEVELS ")) {
			int r = sscanf(str, "LEVELS %d", &this->levelscount);
			if(r != 1) {
				TraceLog(LOG_ERROR, "PIE READ LEVELS line %d ret %d", snum, r);
				return false;
			}
			this->levels = (PIElevel*)malloc(sizeof(PIElevel)*this->levelscount);
			if(this->levels == NULL) {
				TraceLog(LOG_ERROR, "PIE READ LEVELS %d line %d ret %d", this->levelscount, snum, r);
				TraceLog(LOG_ERROR, "Failed to allocate levels!!! [%s]", strerror(errno));
				return false;
			}
			for(int i = 0; i < this->levelscount; i++) {
				this->levels[i].InitAtZero();
			}
		} else if(!strncmpl(str, "LEVEL ")) {
			int newlevel = -1;
			int r = sscanf(str, "LEVEL %d", &nowlevel);
			if(r != 1) {
				TraceLog(LOG_ERROR, "PIE READ LEVEL line %d ret %d", snum, r);
				return false;
			}
			nowlevel = nowlevel - 1;
			TraceLog(LOG_TRACE, "Set level %d", nowlevel);
		} else if(!strncmpl(str, "MATERIALS ")) {
		} else if(!strncmpl(str, "SHADERS ")) {
		} else if(!strncmpl(str, "POINTS ")) {
			if(nowlevel < 0) {
				TraceLog(LOG_ERROR, "PIE READ POINTS line %d level is outside of logical range (%d)", nowlevel);
				return false;
			}
			int r = sscanf(str, "POINTS %d", &this->levels[nowlevel].pointscount);
			if(r != 1) {
				TraceLog(LOG_ERROR, "PIE READ POINTS line %d ret %d level %d", snum, r, nowlevel);
				return false;
			}
			this->levels[nowlevel].points = (Vector3*)malloc(sizeof(Vector3)*this->levels[nowlevel].pointscount);
			if(this->levels[nowlevel].points == NULL) {
				TraceLog(LOG_ERROR, "PIE READ POINTS line %d level %d malloc failed, no memory left?!", snum, nowlevel);
				return false;
			}
			TraceLog(LOG_TRACE, "Allocated points for level %d at %#016x", nowlevel, this->levels[nowlevel].points);
			for(int pointnum = 0; pointnum < this->levels[nowlevel].pointscount; pointnum++) {
				str = strchr(str, '\n');
				if(str == NULL) {
					TraceLog(LOG_ERROR, "PIE READ POINTS unexpected eof, line %d level %d", snum, nowlevel);
					return false;
				}
				str++;
				int pr = sscanf(str, "\t%f %f %f", &this->levels[nowlevel].points[pointnum].x, &this->levels[nowlevel].points[pointnum].y, &this->levels[nowlevel].points[pointnum].z);
				if(pr != 3) {
					TraceLog(LOG_ERROR, "PIE READ POINTS line %d level %d sscanf failed %d", snum, nowlevel, pr);
					return false;
				}
				snum++;
			}
		} else if(!strncmpl(str, "NORMALS ")) {
			if(nowlevel < 0) {
				TraceLog(LOG_ERROR, "PIE READ NORMALS line %d level is outside of logical range (%d)", nowlevel);
				return false;
			}
			int r = sscanf(str, "NORMALS %d", &this->levels[nowlevel].normalscount);
			if(r != 1) {
				TraceLog(LOG_ERROR, "PIE READ NORMALS line %d ret %d level %d", snum, r, nowlevel);
				return false;
			}
			this->levels[nowlevel].normals = (Vector3*)malloc(sizeof(Vector3)*this->levels[nowlevel].normalscount*3);
			if(this->levels[nowlevel].normals == NULL) {
				TraceLog(LOG_ERROR, "PIE READ NORMALS line %d ret %d level %d malloc failed, no memory left?!", snum, r, nowlevel);
				return false;
			}
			for(int normalnum = 0; normalnum < this->levels[nowlevel].normalscount; normalnum++) {
				str = strchr(str, '\n');
				if(str == NULL) {
					TraceLog(LOG_ERROR, "PIE READ NORMALS unexpected eof, line %d level %d", snum, nowlevel);
					return false;
				}
				str++;
				int pr = sscanf(str, "\t%f %f %f %f %f %f %f %f %f",
						&this->levels[nowlevel].normals[normalnum*3+0].x, 
						&this->levels[nowlevel].normals[normalnum*3+0].y, 
						&this->levels[nowlevel].normals[normalnum*3+0].z, 
						&this->levels[nowlevel].normals[normalnum*3+1].x, 
						&this->levels[nowlevel].normals[normalnum*3+1].y, 
						&this->levels[nowlevel].normals[normalnum*3+1].z, 
						&this->levels[nowlevel].normals[normalnum*3+2].x, 
						&this->levels[nowlevel].normals[normalnum*3+2].y, 
						&this->levels[nowlevel].normals[normalnum*3+2].z);
				if(pr != 9) {
					TraceLog(LOG_ERROR, "PIE READ NORMALS line %d level %d sscanf failed %d", snum, nowlevel, pr);
					return false;
				}
				snum++;
			}
		} else if(!strncmpl(str, "POLYGONS ")) {
			if(nowlevel < 0) {
				TraceLog(LOG_ERROR, "PIE READ POLYGONS line %d level is outside of logical range (%d)", nowlevel);
				return false;
			}
			int r = sscanf(str, "POLYGONS %d", &this->levels[nowlevel].polygonscount);
			if(r != 1) {
				TraceLog(LOG_ERROR, "PIE READ POLYGONS line %d ret %d level %d", snum, r, nowlevel);
				return false;
			}
			this->levels[nowlevel].polygons = (PIEpolygon*)malloc(sizeof(PIEpolygon)*this->levels[nowlevel].polygonscount);
			if(this->levels[nowlevel].polygons == NULL) {
				TraceLog(LOG_ERROR, "PIE READ POLYGONS line %d ret %d level %d malloc failed, no memory left?!", snum, r, nowlevel);
				return false;
			}
			for(int polygonnum = 0; polygonnum < this->levels[nowlevel].polygonscount; polygonnum++) {
				str = strchr(str, '\n');
				if(str == NULL) {
					TraceLog(LOG_ERROR, "PIE READ NORMALS unexpected eof, line %d level %d", snum, nowlevel);
					return false;
				}
				str++;
				int pr = sscanf(str, "\t%d %d", &this->levels[nowlevel].polygons[polygonnum].flags, &this->levels[nowlevel].polygons[polygonnum].pcount);
				if(pr != 2) {
					TraceLog(LOG_ERROR, "PIE READ POLYGONS line %d level %d sscanf flag failed %d", snum, nowlevel, pr);
					return false;
				}
				int ffff = this->levels[nowlevel].polygons[polygonnum].flags;
				if(ffff != 200 && ffff != 4200) {
					TraceLog(LOG_ERROR, "PIE READ POLYGONS line %d level %d flag is not 200 %d", snum, nowlevel, this->levels[nowlevel].polygons[polygonnum].flags);
					return false;
				}
				if(this->levels[nowlevel].polygons[polygonnum].pcount != 3) {
					TraceLog(LOG_ERROR, "PIE READ POLYGONS line %d level %d wrong polygon pcount %d", snum, nowlevel, this->levels[nowlevel].polygons[polygonnum].pcount);
					return false;
				}
				int prr = sscanf(str, "\t%*d %*d %d %d %d", &this->levels[nowlevel].polygons[polygonnum].porder[0],
															 &this->levels[nowlevel].polygons[polygonnum].porder[1],
															 &this->levels[nowlevel].polygons[polygonnum].porder[2]);
				if(prr != 3) {
					TraceLog(LOG_ERROR, "PIE READ POLYGONS line %d level %d sscanf porder failed %d", snum, nowlevel, prr);
					return false;
				}
				const char* polyformat;
				if(ffff == 4000 || ffff == 4200) {
					polyformat = "\t%*d %*d %*d %*d %*d %*d %*d %*d %*d %f %f %f %f %f %f";
				} else {
					polyformat = "\t%*d %*d %*d %*d %*d %f %f %f %f %f %f";
				}
				int prrr = sscanf(str, polyformat,
					&this->levels[nowlevel].polygons[polygonnum].texcoords[0],
					&this->levels[nowlevel].polygons[polygonnum].texcoords[1],
					&this->levels[nowlevel].polygons[polygonnum].texcoords[2],
					&this->levels[nowlevel].polygons[polygonnum].texcoords[3],
					&this->levels[nowlevel].polygons[polygonnum].texcoords[4],
					&this->levels[nowlevel].polygons[polygonnum].texcoords[5]);
				if(prrr != 6) {
					TraceLog(LOG_ERROR, "PIE READ POLYGONS line %d level %d sscanf texcoords failed %d", snum, nowlevel, prrr);
					return false;
				}
				snum++;
			}
		} else if(!strncmpl(str, "CONNECTORS ")) {
		} else if(!strncmpl(str, "ANIMOBJECT ")) {
		}
		snum++;
	}
	return true;
}

std::map<std::string, PIEmodel*> loaded_models;

PIEmodel* GetModel(std::string filename) {
	if(loaded_models.count(filename)) {
		return loaded_models[filename];
	}
	PIEmodel* l = new PIEmodel();
	char* c = ReadPHYSFSFile(filename.c_str());
	if(c == NULL) {
		TraceLog(LOG_ERROR, "Failed to load model [%s]", filename.c_str());
		return NULL;
	}
	if(!l->ReadPIE(c)) {
		TraceLog(LOG_ERROR, "Failed to load model [%s]", filename.c_str());
		return NULL;
	}
	loaded_models[filename] = l;
	free(c);
	return l;
}

void FreeModels() {
	for(auto s : loaded_models) {
		loaded_models[s.first]->~PIEmodel();
		loaded_models.erase(s.first);
	}
}


bool LoadMeshFromPIE(PIEmodel* model, Mesh* ret) {
	if(model == nullptr || ret == nullptr) {
		return false;
	}
	if(model->levelscount <= 0) {
		TraceLog(LOG_ERROR, "Model have negative levels?!");
		return false;
	}
	// this->TexturePath = model->texturename;
	// std::vector<glm::vec3> points;
	// struct PIEpolygon {
	// 	int flags;
	// 	int pcount;
	// 	int porder[6];
	// 	float texcoords[12];
	// };
	// std::vector<PIEpolygon> polygons;
	float TexCoordFix = 1.0f;
	if(model->ver != 3) {
		TexCoordFix = 4.0f;
	}
	ret->vertexCount = model->levels[0].polygonscount*3;
	ret->triangleCount = model->levels[0].polygonscount;
	ret->vertices = (float*)MemAlloc(model->levels[0].polygonscount*3*sizeof(float)*3);
	ret->colors = (unsigned char*)MemAlloc(model->levels[0].polygonscount*3*sizeof(unsigned char)*4);
	ret->texcoords = (float*)MemAlloc(model->levels[0].polygonscount*3*sizeof(float)*2);
	size_t filledVertex = 0;
	size_t filledColor = 0;
	size_t filledTex = 0;
	for(unsigned int i = 0; i < model->levels[0].polygonscount; i++) {
		PIEpolygon polygon = model->levels[0].polygons[i];
		if(polygon.pcount != 3) {
			TraceLog(LOG_ERROR, "Object3d does not support pcount other than 3");
			MemFree(ret->vertices);
			MemFree(ret->texcoords);
			return false;
		}
		for(int j = 0; j < polygon.pcount; j++) {
			ret->vertices[filledVertex+0] = model->levels[0].points[polygon.porder[j]].x;
			ret->vertices[filledVertex+1] = model->levels[0].points[polygon.porder[j]].y;
			ret->vertices[filledVertex+2] = model->levels[0].points[polygon.porder[j]].z;
			ret->texcoords[filledTex+0] = polygon.texcoords[j*2+0]*TexCoordFix;
			ret->texcoords[filledTex+1] = polygon.texcoords[j*2+1]*TexCoordFix;
			ret->colors[filledColor+0] = 255;
			ret->colors[filledColor+1] = 255;
			ret->colors[filledColor+2] = 255;
			ret->colors[filledColor+3] = 255;
			filledColor += 4;
			filledVertex += 3;
			filledTex += 2;
		}
	}
	return true;
}