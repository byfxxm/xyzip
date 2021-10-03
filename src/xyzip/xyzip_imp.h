#pragma once

struct file_head
{
	char tag[8] = HEAD_TAG;
	long long size = 0;
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
	bool __pop_file(file_head&);

	ofstream __zip_file;
	ifstream __unzip_file;
};