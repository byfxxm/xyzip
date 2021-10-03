#include "pch.h"
#include "xyzip_imp.h"

bool xyzip_imp::zip(const char* path)
{
	directory_entry __zip_entry(path);

	if (!__zip_entry.exists())
		return false;

	auto pa = __zip_entry.path();
	__zip_file.open(pa.parent_path().wstring() + L"\\" + pa.filename().wstring() + EXTENSION, ios::out | ios::binary);

	if (__zip_entry.is_directory())
		__push_directory(path);
	else if (__zip_entry.is_regular_file())
		__push_file(path);

	if (__zip_file.is_open())
		__zip_file.close();

	return true;
}

bool xyzip_imp::unzip(const char* path)
{
	return true;
}

void xyzip_imp::__push_file(path pa)
{
	assert(directory_entry(pa).is_regular_file());
	assert(__zip_file.is_open());
	
	__zip_file << ZIP_HEAD;
	__zip_file << pa;

	char buff[1024] = { 0 };
	ifstream fin(pa, ios::in | ios::binary);
	while (!fin.eof())
	{
		fin.read(buff, sizeof(buff));
		__zip_file.write(buff, fin.gcount());
	}

	__zip_file << ZIP_TAIL;
	__zip_file.flush();
}

void xyzip_imp::__push_directory(path pa)
{
	assert(directory_entry(pa).is_directory());
	assert(__zip_file.is_open());

	for (auto& file : directory_iterator(pa))
	{
		if (file.is_directory())
			__push_directory(file);

		if (file.is_regular_file())
			__push_file(file);
	}
}