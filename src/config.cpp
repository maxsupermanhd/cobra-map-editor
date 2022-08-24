#include "config.h"

#include <libconfig.h>
#include <raylib.h>

#include <stdlib.h>
#include <filesystem>

std::string appconfigfilename = "cobraeditor.conf";
configuration conf;

bool ScanConfig(config_t* c);
bool PopulateConfig(config_t* c);

bool LoadConfig() {
	std::string confdir = GetAppConfigDir();
	std::error_code err;
	if(!std::filesystem::exists(confdir, err)) {
		if(!std::filesystem::create_directories(confdir, err)) {
			TraceLog(LOG_ERROR, "Failed to create directories: %s (%s)", err.message().c_str(), confdir.c_str());
		}
		return false;
	}
	std::string confpath = confdir + appconfigfilename;
	if(!std::filesystem::exists(confpath, err)) {
		return false;
	}
	config_t c = {0};
	config_init(&c);
	if(config_read_file(&c, confpath.c_str()) != CONFIG_TRUE) {
		TraceLog(LOG_ERROR, "Failed read config file: %s (line %d)", config_error_text(&c), config_error_line(&c));
		config_destroy(&c);
		return false;
	}
	if(!ScanConfig(&c)) {
		TraceLog(LOG_ERROR, "Failed to scan config!");
		config_destroy(&c);
		return false;
	}
	config_destroy(&c);
	return true;
}

bool ScanConfig(config_t* c) {
	const char* wzdata = NULL;
	if(config_lookup_string(c, "wzdata", &wzdata) != CONFIG_TRUE) {
		TraceLog(LOG_ERROR, "Failed to lookup wzdata in config");
		return false;
	}
	conf.wzdata = wzdata;
	config_setting_t *sroot = config_root_setting(c);
	config_setting_t* smpoints = config_setting_lookup(sroot, "mountpaths");
	if(smpoints == NULL) {
		TraceLog(LOG_ERROR, "Failed to lookup mountpaths in config");
		return false;
	}
	if(!config_setting_is_array(smpoints)) {
		TraceLog(LOG_ERROR, "mountpaths is not an array");
		return false;
	}
	int smpointslen = config_setting_length(smpoints);
	if(smpointslen > 0) {
		conf.mountpaths.clear();
		for(int i = 0; i < smpointslen; i++) {
			const char* mpoint = config_setting_get_string_elem(smpoints, i);
			if(mpoint == NULL) {
				TraceLog(LOG_ERROR, "mountpoint setting string get error");
				return false;
			}
			conf.mountpaths.push_back(mpoint);
		}
	}
	return true;
}

bool SaveConfig() {
	std::string confdir = GetAppConfigDir();
	std::error_code err;
	if(!std::filesystem::exists(confdir, err)) {
		if(!std::filesystem::create_directories(confdir, err)) {
			TraceLog(LOG_ERROR, "Failed to create directories: %s (%s)", err.message().c_str(), confdir.c_str());
			return false;
		}
	}
	std::string confpath = confdir + appconfigfilename;
	config_t c = {0};
	config_init(&c);
	if(!PopulateConfig(&c)) {
		TraceLog(LOG_ERROR, "Failed to populate config");
		config_destroy(&c);
		return false;
	}
	if(config_write_file(&c, confpath.c_str()) != CONFIG_TRUE) {
		TraceLog(LOG_ERROR, "Failed to write file");
		config_destroy(&c);
		return false;
	}
	config_destroy(&c);
	return true;
}

bool PopulateConfig(config_t* c) {
	config_setting_t *sroot = config_root_setting(c);
	config_setting_t *swzdata = config_setting_add(sroot, "wzdata", CONFIG_TYPE_STRING);
	if(swzdata == NULL) {
		TraceLog(LOG_ERROR, "Failed to add setting wzdata");
		return false;
	}
	if(config_setting_set_string(swzdata, conf.wzdata.c_str()) != CONFIG_TRUE) {
		TraceLog(LOG_ERROR, "Failed to set setting wzdata");
		return false;
	}
	config_setting_t *smpoints = config_setting_add(sroot, "mountpaths", CONFIG_TYPE_ARRAY);
	if(smpoints == NULL) {
		TraceLog(LOG_ERROR, "Failed to add setting mountpaths");
		return false;
	}
	for(std::string v : conf.mountpaths) {
		config_setting_t *mp = config_setting_add(smpoints, NULL, CONFIG_TYPE_STRING);
		if(mp == NULL) {
			TraceLog(LOG_ERROR, "Failed to add setting to mountpaths");
			return false;
		}
		if(!config_setting_set_string(mp, v.c_str())) {
			TraceLog(LOG_ERROR, "Failed to add setting to mountpaths");
			return false;
		}
	}
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
		return ret;
	}
	home = getenv("HOME");
	if(home) {
		ret = home;
		ret += "/.config/cobra-map-editor/";
		return ret;
	}
	return ret;
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
