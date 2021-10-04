#include "pch.h"
#include "xyzip_imp.h"

bool xyzip_imp::zip(const char* path, const char* directory)
{
	directory_entry path_entry(path);
	directory_entry directory_entry(directory);

	if ((!path_entry.is_regular_file() && !path_entry.is_directory()))
		return false;

	if (!directory_entry.is_directory() && !create_directory(directory_entry))
		return false;

	__zip_file_path = directory_entry.path().wstring() + L"\\" + path_entry.path().filename().wstring() + EXTENSION;
	__zip_file.open(__zip_file_path, ios::out | ios::binary);
	{
		if (path_entry.is_directory())
			__push_directory(path_entry);
		else if (path_entry.is_regular_file())
			__push_file(path_entry);
		else
			assert(0);
	}
	__zip_file.close();

	return true;
}

bool xyzip_imp::unzip(const char* file, const char* directory)
{
	directory_entry file_entry(file);
	directory_entry directory_entry(directory);

	if (!file_entry.is_regular_file())
		return false;

	if (!directory_entry.is_directory() && !create_directory(directory_entry))
		return false;

	bool ret = true;
	__unzip_directory = directory;

	try
	{
		__unzip_file.open(file_entry, ios::in | ios::binary);
		while (__pop_file());
	}
	catch (exception ex)
	{
		cout << ex.what() << endl;
		ret = false;
	}

	__unzip_file.close();
	return ret;
}

void xyzip_imp::__push_file(const directory_entry& file_entry)
{
	assert(file_entry.is_regular_file());
	assert(__zip_file.is_open());

	if (file_entry.path() == __zip_file_path)
		return;
	
	file_head file;
	file.size = file_entry.file_size();
	file.path_len = file_entry.path().string().length();

	__zip_file.write((char*)&file, sizeof(file));
	__zip_file.write(file_entry.path().string().c_str(), file.path_len);

	char buff[BUFF_SIZE] = { 0 };
	ifstream fin(file_entry, ios::in | ios::binary);
	while (!fin.eof())
	{
		fin.read(buff, sizeof(buff));
		__zip_file.write(buff, fin.gcount());
	}

	__zip_file.flush();
}

void xyzip_imp::__push_directory(const directory_entry& directory_entry)
{
	assert(directory_entry.is_directory());
	assert(__zip_file.is_open());

	for (auto& path_entry : directory_iterator(directory_entry))
	{
		if (path_entry.is_directory())
			__push_directory(path_entry);
		else if (path_entry.is_regular_file())
			__push_file(path_entry);
		else
			assert(0);
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