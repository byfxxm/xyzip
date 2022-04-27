#pragma once

namespace xyzip {
	struct FileHead {
		unsigned tag = kFileTag;
		unsigned path_len = 0;
		unsigned long long file_len = 0;
	};

	struct RleHead {
		unsigned tag = kRleTag;
		unsigned count = 0;
		unsigned data = 0;
	};

	class XyzipImp {
	public:
		XyzipImp();
		bool Zip(const char*, const char*);
		bool Unzip(const char*, const char*);
		void SetKey(unsigned);

	private:
		void PushFile(const path&);
		void PushDirectory(const path&);
		bool PopFile();
		void Compress(std::ofstream&, std::ifstream&) const;
		void Decompress(std::ofstream&, std::ifstream&, FileHead&) const;
		void EncodeWrite(std::ofstream&, const char*, std::streamsize = kStep) const;
		void DecodeRead(std::ifstream&, char*, std::streamsize = kStep) const;
		unsigned Encrypt(unsigned, unsigned) const;
		unsigned Decrypt(unsigned, unsigned) const;
		void GenerateLevel();

	private:
		std::ofstream zip_file_;
		path zip_file_dest_;
		path zip_root_;
		std::ifstream unzip_file_;
		path unzip_dir_dest_;
		unsigned key_ = 'xxm';
		unsigned level_ = 0;
	};
}