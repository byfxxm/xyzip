#include "pch.h"
#include "xyzip_imp.h"

inline xyzip_imp& instance()
{
	static xyzip_imp inst;
	return inst;
}

bool zip(const char* path)
{
	return instance().zip(path);
}

bool unzip(const char* path)
{
	return instance().unzip(path);
}