#pragma once

namespace xyzip
{
	struct file_head
	{
		uint32_t tag = FILE_TAG;
		uint32_t path_len = 0;
		uint64_t file_len = 0;
	};

	struct rle_head
	{
		uint32_t tag = RLE_TAG;
		uint32_t count = 0;
		uint32_t data = 0;
	};

	class xyzip_imp
	{
	public:
		xyzip_imp();
		bool zip(const char*, const char*);
		bool unzip(const char*, const char*);
		void setk(uint32_t);

	private:
		void __push_file(const path&);
		void __push_directory(const path&);
		bool __pop_file();
		void __compress(std::ofstream&, std::ifstream&) const;
		void __decompress(std::ofstream&, std::ifstream&, file_head&) const;
		void __encode_write(std::ofstream&, const char*, std::streamsize = STEP) const;
		void __decode_read(std::ifstream&, char*, std::streamsize = STEP) const;
		uint32_t __encrypt(uint32_t, uint32_t) const;
		uint32_t __decrypt(uint32_t, uint32_t) const;
		void __generate_level();

	private:
		std::ofstream __zip_file;
		path __zip_file_dest;
		path __zip_root;
		std::ifstream __unzip_file;
		path __unzip_dir_dest;
		uint32_t __key = 'xxm';
		uint32_t __level = 0;
	};
}