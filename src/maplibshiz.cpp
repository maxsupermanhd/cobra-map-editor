#include "maplibshiz.h"

#include <raylib.h>
#include <wzmaplib/map.h>
#include <ZipIOProvider.h>

#include <iostream>
#include <sstream>
#include <string>

class MapToolDebugLogger : public WzMap::LoggingProtocol
{
public:
	MapToolDebugLogger(bool verbose)
	: verbose(verbose)
	{ }
	virtual ~MapToolDebugLogger() { }
	virtual void printLog(WzMap::LoggingProtocol::LogLevel level, const char *function, int line, const char *str) override {
		switch (level) {
			case WzMap::LoggingProtocol::LogLevel::Info_Verbose:
				TraceLog(LOG_TRACE, "[%s:%d] %s", function, line, str);
				break;
			case WzMap::LoggingProtocol::LogLevel::Info:
				if (!verbose) { return; }
				TraceLog(LOG_INFO, "[%s:%d] %s", function, line, str);
				break;
			case WzMap::LoggingProtocol::LogLevel::Warning:
				TraceLog(LOG_WARNING, "[%s:%d] %s", function, line, str);
				break;
			case WzMap::LoggingProtocol::LogLevel::Error:
				TraceLog(LOG_ERROR, "[%s:%d] %s", function, line, str);
				break;
		}
	}
private:
	bool verbose = false;
};

std::shared_ptr<MapToolDebugLogger> logger = std::make_shared<MapToolDebugLogger>(MapToolDebugLogger(true));

std::unique_ptr<WzMap::MapPackage> loadMapPackage(const char* pathToWzPackage) {
	auto zipArchive = WzMapZipIO::openZipArchiveFS(pathToWzPackage);
	if (!zipArchive) {
		TraceLog(LOG_ERROR, "Failed to open map archive: %s", pathToWzPackage);
		return nullptr;
	}
	auto wzMapPackage = WzMap::MapPackage::loadPackage("/", logger, zipArchive);
	if (!wzMapPackage) {
		TraceLog(LOG_ERROR, "Failed to load map archive package");
		return nullptr;
	}
	return wzMapPackage;
}
