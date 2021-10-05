#include "pch.h"
#include "xyzip_imp.h"

bool xyzip_imp::zip(const char* path, const char* directory)
{
	directory_entry path_entry(path);
	directory_entry dir_entry(directory);

	if ((!path_entry.is_regular_file() && !path_entry.is_directory()))
		return false;

	if (!dir_entry.is_directory() && !create_directory(dir_entry))
		return false;

	__zip_file_dest = dir_entry.path().wstring() + L"\\" + path_entry.path().filename().wstring() + EXT;
	__zip_file.open(__zip_file_dest, ios::out | ios::binary | ios::trunc);
	{
		if (path_entry.is_directory())
		{
			__zip_root = path_entry;
			__push_directory(path_entry);
		}
		else if (path_entry.is_regular_file())
		{
			__zip_root = path_entry.path().parent_path();
			__push_file(path_entry);
		}
		else
			assert(0);
	}
	__zip_file.close();

	return true;
}

bool xyzip_imp::unzip(const char* file, const char* directory)
{
	directory_entry file_entry(file);
	directory_entry dir_entry(directory);

	if (!file_entry.is_regular_file())
		return false;

	if (!dir_entry.is_directory() && !create_directory(dir_entry))
		return false;

	bool ret = true;
	__unzip_dir_dest = directory;

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

void xyzip_imp::setk(unsigned k)
{
	key = k;
}

void xyzip_imp::__push_file(const directory_entry& file_entry)
{
	assert(file_entry.is_regular_file());
	assert(__zip_file.is_open());

	if (file_entry.path() == __zip_file_dest)
		return;
	
	file_head file_h;
	file_h.file_len = file_entry.file_size();

	string pa = file_entry.path().string().substr(__zip_root.string().length());
	file_h.path_len = pa.length();

	__zip_file.write(&BYTE_CAST(file_h), sizeof(file_h));
	__zip_file.write(pa.c_str(), file_h.path_len);

	ifstream fin(file_entry, ios::in | ios::binary);
	__compress(__zip_file, fin);

	__zip_file.flush();
}

void xyzip_imp::__push_directory(const directory_entry& dir_entry)
{
	assert(dir_entry.is_directory());
	assert(__zip_file.is_open());

	for (auto& path_entry : directory_iterator(dir_entry))
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
	file_head file_h;

	__unzip_file.read(&BYTE_CAST(file_h), sizeof(file_head));
	if (__unzip_file.eof())
		return false;

	if (file_h.tag != FILE_TAG)
		throw exception("unzip file error");

	char buff[MAX_PATH] = { 0 };
	__unzip_file.read(buff, file_h.path_len);
	path path_ = __unzip_dir_dest.wstring() + path(buff).wstring();

	if (!exists(path_.parent_path()))
		create_directories(path_.parent_path());

	ofstream fout(path_, ios::out | ios::binary);
	__decompress(fout, __unzip_file, file_h);

	return true;
}

void xyzip_imp::__compress(ofstream& fout, ifstream& fin) const
{
	assert(fin.is_open() && fout.is_open());

	auto write_rle = [this](ofstream& fout, const rle_head& rle_h)
	{
		if (rle_h.count == 0)
			return;

		if (rle_h.count > 1)
		{
			__encode_write(fout, &BYTE_CAST(rle_h.tag));
			__encode_write(fout, &BYTE_CAST(rle_h.count));
		}

		__encode_write(fout, &BYTE_CAST(rle_h.data));
	};

	rle_head rle_h;
	unsigned input = 0;

	for (; ; ++rle_h.count, rle_h.data = input)
	{
		fin.read(&BYTE_CAST(input), STEP);

		if (fin.eof())
		{
			write_rle(fout, rle_h);
			__encode_write(fout, &BYTE_CAST(input), fin.gcount());
			break;
		}

		if (input == rle_h.data || rle_h.count == 0)
			continue;

		write_rle(fout, rle_h);
		rle_h.count = 0;
	}
}

void xyzip_imp::__decompress(ofstream& fout, ifstream& fin, file_head& file_h) const
{
	assert(fin.is_open() && fout.is_open());

	rle_head rle_h;
	unsigned input = 0;
	auto left = file_h.file_len;

	while (left >= STEP)
	{
		__decode_read(fin, &BYTE_CAST(input));

		if (input != RLE_TAG)
		{
			fout.write(&BYTE_CAST(input), STEP);
			left -= fin.gcount();
			continue;
		}

		__decode_read(fin, &BYTE_CAST(rle_h.count));
		__decode_read(fin, &BYTE_CAST(rle_h.data));

		for (unsigned i = 0; i < rle_h.count; ++i)
		{
			fout.write(&BYTE_CAST(rle_h.data), STEP);
		}

		left -= (decltype(left))STEP * rle_h.count;
	}

	__decode_read(fin, &BYTE_CAST(input), left);
	fout.write(&BYTE_CAST(input), left);
}

void xyzip_imp::__encode_write(ofstream& fout, const char* str, streamsize count) const
{
	assert(count <= STEP);
	unsigned code = UINT_CAST(*str);

	if (count == STEP)
		code = ~code + key;

	fout.write(&BYTE_CAST(code), count);
}

void xyzip_imp::__decode_read(ifstream& fin, char* str, streamsize count) const
{
	assert(count <= STEP);

	unsigned& code = UINT_CAST(*str);
	fin.read(&BYTE_CAST(code), count);

	if (count == STEP)
		code = ~(code - key);
}