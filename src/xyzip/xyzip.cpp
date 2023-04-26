#include "pch.h"
#include "xyzip.h"
#include "xyzip_imp.h"

inline XyzipImp& Instance() {
	static XyzipImp inst;
	return inst;
}

bool xy_zip(const char* dest, const char* src) {
	return Instance().Zip(dest, src);
}

bool xy_unzip(const char* dest, const char* src) {
	return Instance().Unzip(dest, src);
}

void xy_setk(unsigned key) {
	return Instance().SetKey(key);
}