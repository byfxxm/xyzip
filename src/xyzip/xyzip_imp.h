#pragma once

struct file_head
{
	unsigned long long tag = 0xFABCBCDCDADABABC;
	unsigned long long size = 0;
	int path_len = 0;
};

class xyzip_imp
{
public:
	bool zip(const char* path);
	bool unzip(const char* path);

private:
	void __push_file(path);
	void __push_directory(path);
	bool __pop_file();

	ofstream __zip_file;
	ifstream __unzip_file;
	path __unzip_directory;
};