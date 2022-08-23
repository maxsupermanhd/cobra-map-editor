#pragma once

#include <string>

std::string appconfigfilename = "cobraeditor.conf";

typedef struct configuration {
	std::string wzdata = "";
} configuration;

configuration conf;

bool LoadConfig();
bool SaveConfig();
