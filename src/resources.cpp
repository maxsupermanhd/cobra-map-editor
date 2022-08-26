#include "resources.h"

#include <physfs.h>
#include <raylib.h>

#include <cstring>
#include <errno.h>

char* ReadPHYSFSFile(std::string path) {
	PHYSFS_File *f = PHYSFS_openRead(path.c_str());
	if(f == NULL) {
		TraceLog(LOG_ERROR, "Failed to open file %s (physfs): %s", path.c_str(), PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
		return NULL;
	}
	PHYSFS_sint64 l = PHYSFS_fileLength(f);
	if(l == -1) {
		TraceLog(LOG_ERROR, "Failed to determine file %s size (physfs): %s", path.c_str(), PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
		if(!PHYSFS_close(f)) {
			TraceLog(LOG_ERROR, "Failed to close file %s (physfs): %s", path.c_str(), PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
		}
		return NULL;
	}
	char* buf = (char*)malloc(l+1);
	if(buf == NULL) {
		TraceLog(LOG_ERROR, "Failed to allocate buffer of size %ld for file %s: %s", l, path.c_str(), strerror(errno));
		if(!PHYSFS_close(f)) {
			TraceLog(LOG_ERROR, "Failed to close file %s (physfs): %s", path.c_str(), PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
		}
		return NULL;
	}
	PHYSFS_sint64 r = PHYSFS_readBytes(f, buf, l);
	buf[r] = 0;
	if(r != l) {
		if(PHYSFS_eof(f)) {
			TraceLog(LOG_INFO, "physfs is drunk");
		} else {
			TraceLog(LOG_ERROR, "Failed to read whole file %s length %ld got only %d: %s", path.c_str(), l, r, PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
		}
	}
	if(!PHYSFS_close(f)) {
		TraceLog(LOG_ERROR, "Failed to close file %s (physfs): %s", path.c_str(), PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
	}
	return buf;
}
