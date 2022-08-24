#pragma once

#include <string>
#include <cstring>
#include <stdarg.h>

size_t snprcat(char* str, size_t stroffs, size_t strmax, const char* format, ...);
char* sprcatr(char* str, const char* format, ...);
bool equalstr(char* s1, const char* s2);

inline int strncmpl(std::string a, const char* b) {
    return strncmp(a.c_str(), b, strlen(b));
}
inline int strncmpl(char* a, char* b) {
    return strncmp(a, b, strlen(b));
}
inline int strncmpl(char* a, const char* b) {
    return strncmp(a, b, strlen(b));
}
inline int strncmpl(const char* a, char* b) {
    return strncmp(a, b, strlen(b));
}
inline int strncmpl(const char* a, const char* b) {
    return strncmp(a, b, strlen(b));
}
inline int sscanf(std::string a, const char* format, ...) {
    va_list args;
	va_start(args, format);
    int r = vsscanf(a.c_str(), format, args);
    va_end(args);
    return r;
}

void loggingCallback(int logLevel, const char *text, va_list args);
