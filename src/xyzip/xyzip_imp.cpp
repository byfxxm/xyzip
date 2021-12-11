#include "pch.h"
#include "xyzip_imp.h"

xyzip_imp::xyzip_imp()
{
	__generate_level();
}

bool xyzip_imp::zip(const char* dest, const char* src)
{
	path src_(src);
	path dest_(dest);

	if ((!is_regular_file(src_) && !is_directory(src_)))
		return false;

	if (!is_directory(dest_) && !create_directory(dest_))
		return false;

	__zip_file_dest = dest_.wstring() + L"\\" + src_.filename().wstring() + EXT;
	__zip_file.open(__zip_file_dest, std::ios::out | std::ios::binary | std::ios::trunc);
	{
		if (is_directory(src_))
		{
			__zip_root = src_;
			__push_directory(src_);
		}
		else if (is_regular_file(src_))
		{
			__zip_root = src_.parent_path();
			__push_file(src_);
		}
		else
			assert(0);
	}
	__zip_file.close();

	return true;
}

bool xyzip_imp::unzip(const char* dest, const char* src)
{
	path src_(src);
	path dest_(dest);

	if (!is_regular_file(src_))
		return false;

	if (!is_directory(dest_) && !create_directory(dest_))
		return false;

	bool ret = true;
	__unzip_dir_dest = dest_;

	try
	{
		__unzip_file.open(src_, std::ios::in | std::ios::binary);
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

void xyzip_imp::__push_file(const path& src)
{
	assert(is_regular_file(src));
	assert(__zip_file.is_open());

	if (src == __zip_file_dest)
		return;
	
	file_head file_h;
	file_h.file_len = directory_entry(src).file_size();

	std::string path_str = src.string().substr(__zip_root.string().length());
	file_h.path_len = (uint32_t)path_str.length();

	__encode_write(__zip_file, &CHAR_CAST(file_h), sizeof(file_h));
	__encode_write(__zip_file, path_str.c_str(), file_h.path_len);

	std::ifstream fin(src, std::ios::in | std::ios::binary);
	__compress(__zip_file, fin);

	__zip_file.flush();
}

void xyzip_imp::__push_directory(const path& src)
{
	assert(is_directory(src));
	assert(__zip_file.is_open());

	for (auto& path_entry : directory_iterator(src))
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
	__decode_read(__unzip_file, &CHAR_CAST(file_h), sizeof(file_h));

	if (file_h.tag != FILE_TAG)
		throw std::exception("unzip file error");

	char buff[MAX_PATH] = { 0 };
	__decode_read(__unzip_file, buff, file_h.path_len);
	path path_ = __unzip_dir_dest.wstring() + path(buff).wstring();

	if (!exists(path_.parent_path()))
		create_directories(path_.parent_path());

	std::ofstream fout(path_, std::ios::out | std::ios::binary);
	__decompress(fout, __unzip_file, file_h);

	return true;
}

void xyzip_imp::__compress(std::ofstream& fout, std::ifstream& fin) const
{
	assert(fin.is_open() && fout.is_open());

	auto write_rle = [this](std::ofstream& fout, const rle_head& rle_h)
	{
		if (rle_h.count < 3)
		{
			for (uint32_t i = 0; i < rle_h.count; ++i)
				__encode_write(fout, &CHAR_CAST(rle_h.data));

			return;
		}

		__encode_write(fout, &CHAR_CAST(rle_h.tag));
		__encode_write(fout, &CHAR_CAST(rle_h.count));
		__encode_write(fout, &CHAR_CAST(rle_h.data));
	};

	rle_head rle_h;
	uint32_t input = 0;

	for (; ; ++rle_h.count, rle_h.data = input)
	{
		fin.read(&CHAR_CAST(input), STEP);

		if (fin.eof())
		{
			write_rle(fout, rle_h);
			__encode_write(fout, &CHAR_CAST(input), fin.gcount());
			break;
		}

		if (input == rle_h.data || rle_h.count == 0)
			continue;

		write_rle(fout, rle_h);
		rle_h.count = 0;
	}
}

void xyzip_imp::__decompress(std::ofstream& fout, std::ifstream& fin, file_head& file_h) const
{
	assert(fin.is_open() && fout.is_open());

	rle_head rle_h;
	uint32_t input = 0;
	auto left = file_h.file_len;

	while (left >= STEP)
	{
		__decode_read(fin, &CHAR_CAST(input));

		if (input != RLE_TAG)
		{
			fout.write(&CHAR_CAST(input), STEP);
			left -= fin.gcount();
			continue;
		}

		__decode_read(fin, &CHAR_CAST(rle_h.count));
		__decode_read(fin, &CHAR_CAST(rle_h.data));

		for (uint32_t i = 0; i < rle_h.count; ++i)
			fout.write(&CHAR_CAST(rle_h.data), STEP);

		left -= (decltype(left))STEP * rle_h.count;
	}

	__decode_read(fin, &CHAR_CAST(input), left);
	fout.write(&CHAR_CAST(input), left);
}

void xyzip_imp::__encode_write(std::ofstream& fout, const char* str, std::streamsize count) const
{
	uint32_t idx = 0;
	uint32_t code = 0;

	for (; idx < count; idx += STEP)
	{
		code = __encrypt(UINT32_CAST(str[idx]), __level);
		fout.write(&CHAR_CAST(code), min(STEP, count - idx));
	}
}

void xyzip_imp::__decode_read(std::ifstream& fin, char* str, std::streamsize count) const
{
	uint32_t idx = 0;
	uint32_t code = 0;
	uint32_t len = 0;

	for (; idx < count; idx += STEP)
	{
		len = min(STEP, (uint32_t)count - idx);
		fin.read(&CHAR_CAST(code), len);
		auto temp = __decrypt(code, __level);
		memcpy(&str[idx], &temp, len);
	}
}

inline uint32_t xyzip_imp::__encrypt(uint32_t code, uint32_t level) const
{
	for (uint32_t i = 0; i < level; ++i)
		code = ~code + __key ^ __key;

	return code;
}

inline uint32_t xyzip_imp::__decrypt(uint32_t code, uint32_t level) const
{
	for (uint32_t i = 0; i < level; ++i)
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