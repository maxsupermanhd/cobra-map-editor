#include "config.h"

#include <libconfig.h>
#include "raylib.h"

#include <stdlib.h>
#include <filesystem>

bool LoadConfig() {
	std::string confdir = GetAppConfigDir();
	std::error_code err;
	if(!std::filesystem::exists(confdir, err)) {
		if(!std::filesystem::create_directories(confdir, err)) {
			TraceLog(LOG_ERROR, "Failed to create directories: %s (%s)", err.message().c_str(), confdir.c_str());
			return false;
		}
	}
	std::string confpath = confdir + appconfigfilename;
	
	return true;
}


// cross-platform basement
// linux:   /home/user/.config/cobra-map-editor/cobraeditor.conf
// windows: %APPDATA%\cobra-map-editor\cobraeditor.conf (should work™)
// apple:   ???
std::string GetAppConfigDir() {
#if defined(__linux__) || defined(BSD)
	char *home = getenv("XDG_CONFIG_HOME");
	std::string ret = "";
	if(home) {
		ret = home;
		ret += "/cobra-map-editor/";
		return ret + appconfigfilename;
	}
	home = getenv("HOME");
	if(home) {
		ret += "/.config/cobra-map-editor/";
		return ret + appconfigfilename;
	}
	return appconfigfilename;
#elif defined(WIN32)
	#include <shlobj_core.h>
	#include <combaseapi.h>
	PWSTR* p = NULL;
	std::string ret = "";
	if(SUCCESS(SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, NULL, ))) {
		ret += p;
		ret += "\\cobra-map-editor\\";
	}
	CoTaskMemFree(p);
	return ret + appconfigfilename;
#elif defined(__APPLE__)
	return appconfigfilename;
#endif
}
