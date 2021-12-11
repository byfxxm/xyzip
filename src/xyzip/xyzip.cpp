#include "pch.h"
#include "xyzip.h"
#include "xyzip_imp.h"

inline xyzip_imp& instance()
{
	static xyzip_imp inst;
	return inst;
}

bool xy_zip(const char* path, const char* directory)
{
	return instance().zip(path, directory);
}

bool xy_unzip(const char* file, const char* directory)
{
	return instance().unzip(file, directory);
}

void xy_setk(uint32_t key)
{
	return instance().setk(key);
}