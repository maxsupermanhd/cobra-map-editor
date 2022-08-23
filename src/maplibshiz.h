#pragma once

#include <wzmaplib/map_package.h>

#include <memory>

std::unique_ptr<WzMap::MapPackage> loadMapPackage(const char* pathToWzPackage);
