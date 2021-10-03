#include "pch.h"
#include "xyzip_imp.h"

using namespace filesystem;

bool xyzip_imp::zip(const char* path)
{
	for (auto& it : directory_iterator(path))
	{
		cout << it << endl;
	}

	return true;
}

bool xyzip_imp::unzip(const char* path)
{
	return true;
}