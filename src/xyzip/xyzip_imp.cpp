#include "pch.h"
#include "xyzip_imp.h"

XyzipImp::XyzipImp()
{
	GenerateLevel();
}

bool XyzipImp::Zip(const char* dest, const char* src)
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
			PushDirectory(src_);
		}
		else if (is_regular_file(src_))
		{
			__zip_root = src_.parent_path();
			PushFile(src_);
		}
		else
			assert(0);
	}
	__zip_file.close();

	return true;
}

bool XyzipImp::Unzip(const char* dest, const char* src)
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
		while (PopFile());
	}
	catch (std::exception ex)
	{
		std::cout << ex.what() << std::endl;
		ret = false;
	}

	__unzip_file.close();
	return ret;
}

void XyzipImp::SetKey(unsigned k)
{
	__key = k;
	GenerateLevel();
}

void XyzipImp::PushFile(const path& src)
{
	assert(is_regular_file(src));
	assert(__zip_file.is_open());

	if (src == __zip_file_dest)
		return;
	
	FileHead file_h;
	file_h.file_len = directory_entry(src).file_size();

	std::string path_str = src.string().substr(__zip_root.string().length());
	file_h.path_len = (unsigned)path_str.length();

	EncodeWrite(__zip_file, &CHAR_CAST(file_h), sizeof(file_h));
	EncodeWrite(__zip_file, path_str.c_str(), file_h.path_len);

	std::ifstream fin(src, std::ios::in | std::ios::binary);
	Compress(__zip_file, fin);

	__zip_file.flush();
}

void XyzipImp::PushDirectory(const path& src)
{
	assert(is_directory(src));
	assert(__zip_file.is_open());

	for (auto& path_entry : directory_iterator(src))
	{
		if (path_entry.is_directory())
			PushDirectory(path_entry);
		else if (path_entry.is_regular_file())
			PushFile(path_entry);
		else
			assert(0);
	}
}

bool XyzipImp::PopFile()
{
	if (__unzip_file.peek() == EOF)
		return false;

	FileHead file_h;
	DecodeRead(__unzip_file, &CHAR_CAST(file_h), sizeof(file_h));

	if (file_h.tag != FILE_TAG)
		throw std::exception("unzip file error");

	char buff[MAX_PATH]{};
	DecodeRead(__unzip_file, buff, file_h.path_len);
	path path_ = __unzip_dir_dest.wstring() + path(buff).wstring();

	if (!exists(path_.parent_path()))
		create_directories(path_.parent_path());

	std::ofstream fout(path_, std::ios::out | std::ios::binary);
	Decompress(fout, __unzip_file, file_h);

	return true;
}

void XyzipImp::Compress(std::ofstream& fout, std::ifstream& fin) const
{
	assert(fin.is_open() && fout.is_open());

	auto write_rle = [this](std::ofstream& fout, const RleHead& rle_h)
	{
		if (rle_h.count < 3)
		{
			for (unsigned i = 0; i < rle_h.count; ++i)
				EncodeWrite(fout, &CHAR_CAST(rle_h.data));

			return;
		}

		EncodeWrite(fout, &CHAR_CAST(rle_h.tag));
		EncodeWrite(fout, &CHAR_CAST(rle_h.count));
		EncodeWrite(fout, &CHAR_CAST(rle_h.data));
	};

	RleHead rle_h;
	unsigned input = 0;

	for (; ; ++rle_h.count, rle_h.data = input)
	{
		fin.read(&CHAR_CAST(input), STEP);

		if (fin.eof())
		{
			write_rle(fout, rle_h);
			EncodeWrite(fout, &CHAR_CAST(input), fin.gcount());
			break;
		}

		if (input == rle_h.data || rle_h.count == 0)
			continue;

		write_rle(fout, rle_h);
		rle_h.count = 0;
	}
}

void XyzipImp::Decompress(std::ofstream& fout, std::ifstream& fin, FileHead& file_h) const
{
	assert(fin.is_open() && fout.is_open());

	RleHead rle_h;
	unsigned input = 0;
	auto left = file_h.file_len;

	while (left >= STEP)
	{
		DecodeRead(fin, &CHAR_CAST(input));

		if (input != RLE_TAG)
		{
			fout.write(&CHAR_CAST(input), STEP);
			left -= fin.gcount();
			continue;
		}

		DecodeRead(fin, &CHAR_CAST(rle_h.count));
		DecodeRead(fin, &CHAR_CAST(rle_h.data));

		for (unsigned i = 0; i < rle_h.count; ++i)
			fout.write(&CHAR_CAST(rle_h.data), STEP);

		left -= (decltype(left))STEP * rle_h.count;
	}

	DecodeRead(fin, &CHAR_CAST(input), left);
	fout.write(&CHAR_CAST(input), left);
}

void XyzipImp::EncodeWrite(std::ofstream& fout, const char* str, std::streamsize count) const
{
	unsigned idx = 0;
	unsigned code = 0;

	for (; idx < count; idx += STEP)
	{
		code = Encrypt(UINT_CAST(str[idx]), __level);
		fout.write(&CHAR_CAST(code), min(STEP, count - idx));
	}
}

void XyzipImp::DecodeRead(std::ifstream& fin, char* str, std::streamsize count) const
{
	unsigned idx = 0;
	unsigned code = 0;
	unsigned len = 0;

	for (; idx < count; idx += STEP)
	{
		len = min(STEP, (unsigned)count - idx);
		fin.read(&CHAR_CAST(code), len);
		auto temp = Decrypt(code, __level);
		memcpy(&str[idx], &temp, len);
	}
}

inline unsigned XyzipImp::Encrypt(unsigned code, unsigned level) const
{
	for (unsigned i = 0; i < level; ++i)
		code = ~code + __key ^ __key;

	return code;
}

inline unsigned XyzipImp::Decrypt(unsigned code, unsigned level) const
{
	for (unsigned i = 0; i < level; ++i)
		code = ~(code ^ __key) + __key;

	return code;
}

void XyzipImp::GenerateLevel()
{
	auto key = __key;
	__level = 0;

	while (key)
	{
		__level += key & 0x1;
		key >>= 1;
	}
}