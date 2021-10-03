#include "pch.h"
#include "xyzip_imp.h"

bool xyzip_imp::zip(const char* path)
{
	directory_entry entry(path);

	if (!entry.exists())
		return false;

	auto pa = entry.path();
	__zip_file.open(pa.parent_path().wstring() + L"\\" + pa.filename().wstring() + EXTENSION, ios::out | ios::binary);

	if (entry.is_directory())
		__push_directory(path);
	else if (entry.is_regular_file())
		__push_file(path);

	if (__zip_file.is_open())
		__zip_file.close();

	return true;
}

bool xyzip_imp::unzip(const char* path)
{
	directory_entry entry(path);
	__unzip_directory = path;
	__unzip_directory = __unzip_directory.parent_path();

	if (!entry.exists())
		return false;

	bool ret = true;

	try
	{
		__unzip_file.open(path, ios::in | ios::binary);
		while (__pop_file());
	}
	catch (exception ex)
	{
		cout << ex.what() << endl;
		ret = false;
	}

	if (__unzip_file.is_open())
		__unzip_file.close();

	return ret;
}

void xyzip_imp::__push_file(path pa)
{
	assert(directory_entry(pa).is_regular_file());
	assert(__zip_file.is_open());
	
	file_head file;
	file.size = directory_entry(pa).file_size();
	file.path_len = pa.string().length();

	__zip_file.write((char*)&file, sizeof(file));
	__zip_file.write(pa.string().c_str(), file.path_len);

	char buff[BUFF_SIZE] = { 0 };
	ifstream fin(pa, ios::in | ios::binary);

	while (!fin.eof())
	{
		fin.read(buff, sizeof(buff));
		__zip_file.write(buff, fin.gcount());
	}

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

bool xyzip_imp::__pop_file()
{
	if (__unzip_file.eof())
		return false;

	file_head file;
	__unzip_file.read((char*)&file, sizeof(file_head));

	char pa[MAX_PATH] = { 0 };
	__unzip_file.read(pa, file.path_len);
	ofstream fout(__unzip_directory.wstring() + L"\\" + path(pa).filename().wstring(), ios::out | ios::binary);

	auto left = file.size;
	char buff[1024] = { 0 };
	for (auto left = file.size; left; left -= __unzip_file.gcount())
	{
		__unzip_file.read(buff, min(sizeof(buff), left));
		fout.write(buff, __unzip_file.gcount());

		if (left < (decltype(left))__unzip_file.gcount())
			throw exception("file error");
	}

	return true;
}