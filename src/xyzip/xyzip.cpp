#include "pch.h"
#include "xyzip.h"
#include "xyzip_imp.h"

inline XyzipImp& instance()
{
	static XyzipImp inst;
	return inst;
}

bool xy_zip(const char* path, const char* directory)
{
	return instance().Zip(path, directory);
}

bool xy_unzip(const char* file, const char* directory)
{
	return instance().unzip(file, directory);
}

void xy_setk(unsigned key)
{
	return instance().setk(key);
}