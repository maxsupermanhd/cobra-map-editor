#pragma once

#include <string>

char* ReadPHYSFSFile(std::string path);
char* ReadPHYSFSFileLength(std::string path, signed long long& len);