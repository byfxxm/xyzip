#include "pch.h"
#include "xyzip.h"
#include "xyzip_imp.h"

inline xyzip_imp& instance()
{
	static xyzip_imp inst;
	return inst;
}

bool xy_zip(const char* path)
{
	return instance().zip(path);
}

bool xy_unzip(const char* path)
{
	return instance().unzip(path);
}