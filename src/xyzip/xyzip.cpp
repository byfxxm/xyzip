#include "pch.h"
#include "xyzip.h"
#include "xyzip_imp.h"

inline XyzipImp& Instance() {
	static XyzipImp inst;
	return inst;
}

bool xy_zip(const char* path, const char* directory) {
	return Instance().Zip(path, directory);
}

bool xy_unzip(const char* file, const char* directory) {
	return Instance().Unzip(file, directory);
}

void xy_setk(unsigned key) {
	return Instance().SetKey(key);
}