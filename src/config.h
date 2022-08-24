#pragma once

#include <string>
#include <vector>

extern std::string appconfigfilename;

typedef struct configuration {
	std::string wzdata = "";
	std::vector<std::string> mountpaths;
} configuration;

extern configuration conf;

bool LoadConfig();
bool SaveConfig();

std::string GetAppConfigDir();
