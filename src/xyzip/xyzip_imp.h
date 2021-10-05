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
	void setk(unsigned);

private:
	void __push_file(const directory_entry&);
	void __push_directory(const directory_entry&);
	bool __pop_file();

	void __compress(ifstream&, ofstream&) const;
	void __decompress(ifstream&, ofstream&, file_head&) const;
	void __encode_write(ofstream&, const char*, streamsize = STEP) const;
	void __decode_read(ifstream&, char*, streamsize = STEP) const;
	unsigned __encrypt(unsigned, unsigned = LEVEL) const;
	unsigned __decrypt(unsigned, unsigned = LEVEL) const;

	ofstream __zip_file;
	path __zip_file_dest;
	path __zip_root;
	ifstream __unzip_file;
	path __unzip_dir_dest;
	unsigned __key = 'xxm';
};