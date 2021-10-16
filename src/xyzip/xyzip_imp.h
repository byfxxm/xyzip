#pragma once
#include "huffman_alphabet.h"

struct file_head
{
	unsigned tag = FILE_TAG;
	unsigned path_len = 0;
	unsigned long long file_len = 0;
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
	xyzip_imp();
	bool zip(const char*, const char*);
	bool unzip(const char*, const char*);
	void setk(unsigned);

private:
	void __push_file(const directory_entry&);
	void __push_directory(const directory_entry&);
	bool __pop_file();
	void __generate_alphabet(ifstream&, huffman_alphabet&);

	void __compress(ifstream&, ofstream&) const;
	void __decompress(ifstream&, ofstream&, file_head&) const;
	void __encode_write(ofstream&, const char*, streamsize = STEP) const;
	void __decode_read(ifstream&, char*, streamsize = STEP) const;
	unsigned __encrypt(unsigned, unsigned) const;
	unsigned __decrypt(unsigned, unsigned) const;
	void __generate_level();

	ofstream __zip_file;
	path __zip_file_dest;
	path __zip_root;
	ifstream __unzip_file;
	path __unzip_dir_dest;
	unsigned __key = 'xxm';
	unsigned __level = 0;
};