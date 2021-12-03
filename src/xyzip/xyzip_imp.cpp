#include "pch.h"
#include "xyzip_imp.h"

xyzip_imp::xyzip_imp()
{
	__generate_level();
}

bool xyzip_imp::zip(const char* path, const char* directory)
{
	std::filesystem::directory_entry path_entry(std::filesystem::absolute(path));
	std::filesystem::directory_entry dir_entry(std::filesystem::absolute(directory));

	if ((!path_entry.is_regular_file() && !path_entry.is_directory()))
		return false;

	if (!dir_entry.is_directory() && !create_directory(dir_entry))
		return false;

	__zip_file_dest = dir_entry.path().wstring() + L"\\" + path_entry.path().filename().wstring() + EXT;
	__zip_file.open(__zip_file_dest, std::ios::out | std::ios::binary | std::ios::trunc);
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
	std::filesystem::directory_entry file_entry(std::filesystem::absolute(file));
	std::filesystem::directory_entry dir_entry(std::filesystem::absolute(directory));

	if (!file_entry.is_regular_file())
		return false;

	if (!dir_entry.is_directory() && !create_directory(dir_entry))
		return false;

	bool ret = true;
	__unzip_dir_dest = dir_entry;

	try
	{
		__unzip_file.open(file_entry, std::ios::in | std::ios::binary);
		while (__pop_file());
	}
	catch (std::exception ex)
	{
		std::cout << ex.what() << std::endl;
		ret = false;
	}

	__unzip_file.close();
	return ret;
}

void xyzip_imp::setk(unsigned k)
{
	__key = k;
	__generate_level();
}

void xyzip_imp::__push_file(const std::filesystem::directory_entry& file_entry)
{
	assert(file_entry.is_regular_file());
	assert(__zip_file.is_open());

	if (file_entry.path() == __zip_file_dest)
		return;
	
	file_head file_h;
	file_h.file_len = file_entry.file_size();

	std::string path_str = file_entry.path().string().substr(__zip_root.string().length());
	file_h.path_len = path_str.length();

	__encode_write(__zip_file, &BYTE_CAST(file_h), sizeof(file_h));
	__encode_write(__zip_file, path_str.c_str(), file_h.path_len);

	std::ifstream fin(file_entry.path(), std::ios::in | std::ios::binary);
	__compress(fin, __zip_file);

	__zip_file.flush();
}

void xyzip_imp::__push_directory(const std::filesystem::directory_entry& dir_entry)
{
	assert(dir_entry.is_directory());
	assert(__zip_file.is_open());

	for (auto& path_entry : std::filesystem::directory_iterator(dir_entry))
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
	if (__unzip_file.peek() == EOF)
		return false;

	file_head file_h;
	__decode_read(__unzip_file, &BYTE_CAST(file_h), sizeof(file_h));

	if (file_h.tag != FILE_TAG)
		throw std::exception("unzip file error");

	char buff[MAX_PATH] = { 0 };
	__decode_read(__unzip_file, buff, file_h.path_len);
	std::filesystem::path path_ = __unzip_dir_dest.wstring() + std::filesystem::path(buff).wstring();

	if (!exists(path_.parent_path()))
		create_directories(path_.parent_path());

	std::ofstream fout(path_, std::ios::out | std::ios::binary);
	__decompress(__unzip_file, fout, file_h);

	return true;
}

void xyzip_imp::__compress(std::ifstream& fin, std::ofstream& fout) const
{
	assert(fin.is_open() && fout.is_open());

	auto write_rle = [this](std::ofstream& fout, const rle_head& rle_h)
	{
		if (rle_h.count < 3)
		{
			for (unsigned i = 0; i < rle_h.count; ++i)
				__encode_write(fout, &BYTE_CAST(rle_h.data));

			return;
		}

		__encode_write(fout, &BYTE_CAST(rle_h.tag));
		__encode_write(fout, &BYTE_CAST(rle_h.count));
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

void xyzip_imp::__decompress(std::ifstream& fin, std::ofstream& fout, file_head& file_h) const
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
			fout.write(&BYTE_CAST(rle_h.data), STEP);

		left -= (decltype(left))STEP * rle_h.count;
	}

	__decode_read(fin, &BYTE_CAST(input), left);
	fout.write(&BYTE_CAST(input), left);
}

void xyzip_imp::__encode_write(std::ofstream& fout, const char* str, std::streamsize count) const
{
	unsigned idx = 0;
	unsigned code = 0;

	for (; idx < count; idx += STEP)
	{
		code = __encrypt(UINT_CAST(str[idx]), __level);
		fout.write(&BYTE_CAST(code), min(STEP, count - idx));
	}
}

void xyzip_imp::__decode_read(std::ifstream& fin, char* str, std::streamsize count) const
{
	unsigned idx = 0;
	unsigned code = 0;
	unsigned len = 0;

	for (; idx < count; idx += STEP)
	{
		len = min(STEP, (unsigned)count - idx);
		fin.read(&BYTE_CAST(code), len);
		auto temp = __decrypt(code, __level);
		memcpy(&str[idx], &temp, len);
	}
}

unsigned xyzip_imp::__encrypt(unsigned code, unsigned level) const
{
	for (unsigned i = 0; i < level; ++i)
		code = ~code + __key ^ __key;

	return code;
}

unsigned xyzip_imp::__decrypt(unsigned code, unsigned level) const
{
	for (unsigned i = 0; i < level; ++i)
		code = ~(code ^ __key) + __key;

	return code;
}

void xyzip_imp::__generate_level()
{
	auto key = __key;
	__level = 0;

	while (key)
	{
		__level += key & 0x1;
		key >>= 1;
	}
}