#pragma once

struct file_head
{
	using ull = unsigned long long;

	char tag[8] = HEAD_TAG;
	ull size;
};

class xyzip_imp
{
public:
	bool zip(const char* path);
	bool unzip(const char* path);

private:
	void __push_file(path);
	void __push_directory(path);

	directory_entry __zip_entry;
	ofstream __zip_file;
};