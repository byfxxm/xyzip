#pragma once

namespace xyzip
{
	struct FileHead
	{
		unsigned tag = FILE_TAG;
		unsigned path_len = 0;
		unsigned long long file_len = 0;
	};

	struct RleHead
	{
		unsigned tag = RLE_TAG;
		unsigned count = 0;
		unsigned data = 0;
	};

	class XyzipImp
	{
	public:
		XyzipImp();
		bool Zip(const char*, const char*);
		bool Unzip(const char*, const char*);
		void setk(unsigned);

	private:
		void __push_file(const path&);
		void __push_directory(const path&);
		bool __pop_file();
		void __compress(std::ofstream&, std::ifstream&) const;
		void __decompress(std::ofstream&, std::ifstream&, FileHead&) const;
		void __encode_write(std::ofstream&, const char*, std::streamsize = STEP) const;
		void __decode_read(std::ifstream&, char*, std::streamsize = STEP) const;
		unsigned __encrypt(unsigned, unsigned) const;
		unsigned __decrypt(unsigned, unsigned) const;
		void __generate_level();

	private:
		std::ofstream __zip_file;
		path __zip_file_dest;
		path __zip_root;
		std::ifstream __unzip_file;
		path __unzip_dir_dest;
		unsigned __key = 'xxm';
		unsigned __level = 0;
	};
}