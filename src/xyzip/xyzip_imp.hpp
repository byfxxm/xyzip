#pragma once
#include <cassert>
#include <filesystem>
#include <fstream>
#define CHAR_CAST(var) (*(char *)&(var))
#define UINT_CAST(var) (*(unsigned *)&(var))

constexpr auto kExt = L".xyzip";
constexpr auto kFileTag = 0xFABCBCDC;
constexpr auto kRleTag = 0xFFABCBCD;
constexpr auto kStep = sizeof(unsigned);

namespace xyzip {}
using namespace xyzip;
using namespace std::filesystem;

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

constexpr unsigned Encrypt(unsigned code, unsigned key, unsigned level) {
  for (unsigned i = 0; i < level; ++i)
    code = ~code + key ^ key;

  return code;
}

constexpr unsigned Decrypt(unsigned code, unsigned key, unsigned level) {
  for (unsigned i = 0; i < level; ++i)
    code = ~(code ^ key) + key;

  return code;
}

class XyzipImp {
public:
  XyzipImp();
  bool Zip(const char *, const char *);
  bool Unzip(const char *, const char *);
  void SetKey(unsigned);

private:
  void _PushFile(const path &);
  void _PushDirectory(const path &);
  bool _PopFile();
  void _Compress(std::ofstream &, std::ifstream &) const;
  void _Decompress(std::ofstream &, std::ifstream &, FileHead &) const;
  void _EncodeWrite(std::ofstream &, const char *,
                    std::streamsize = kStep) const;
  void _DecodeRead(std::ifstream &, char *, std::streamsize = kStep) const;
  void _GenerateLevel();

private:
  std::ofstream _zip_file;
  path _zip_file_dest;
  path _zip_root;
  std::ifstream _unzip_file;
  path _unzip_dir_dest;
  unsigned _key = 0x1234BABA;
  unsigned _level = 0;
};
} // namespace xyzip
