#pragma once

struct file_head
{
	unsigned long long tag = 0xFABCBCDCDADABABC;
	unsigned long long file_len = 0;
	int path_len = 0;
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

	ofstream __zip_file;
	path __zip_file_dest;
	ifstream __unzip_file;
	path __unzip_dir_dest;
};