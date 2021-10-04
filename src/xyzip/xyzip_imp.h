#pragma once

struct file_head
{
	unsigned long long tag = FILE_TAG;
	unsigned long long file_len = 0;
	int path_len = 0;
};

struct rle_head
{
	unsigned tag = RLE_TAG;
	unsigned count = 0;
	unsigned data = 0;
};

class xyzip_imp
{
public:
	bool zip(const char*, const char*);
	bool unzip(const char*, const char*);

private:
	void __push_file(const directory_entry&);
	void __push_directory(const directory_entry&);
	bool __pop_file();

	static void __compress(ifstream&, ofstream&);
	static void __decompress(ifstream&, ofstream&, file_head&);

	ofstream __zip_file;
	path __zip_file_dest;
	path __zip_root;
	ifstream __unzip_file;
	path __unzip_dir_dest;
};