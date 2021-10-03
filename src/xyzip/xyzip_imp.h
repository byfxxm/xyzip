#pragma once

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