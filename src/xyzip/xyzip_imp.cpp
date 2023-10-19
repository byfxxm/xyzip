#include "xyzip_imp.hpp"
#include <iostream>

XyzipImp::XyzipImp() { _GenerateLevel(); }

bool XyzipImp::Zip(const char *dest, const char *src) {
  path src_(src);
  path dest_(dest);

  if ((!is_regular_file(src_) && !is_directory(src_)))
    return false;

  if (!is_directory(dest_) && !create_directory(dest_))
    return false;

  _zip_file_dest = dest_.wstring() + L"\\" + src_.filename().wstring() + kExt;
  _zip_file.open(_zip_file_dest,
                 std::ios::out | std::ios::binary | std::ios::trunc);

  if (is_directory(src_)) {
    _zip_root = src_;
    _PushDirectory(src_);
  } else if (is_regular_file(src_)) {
    _zip_root = src_.parent_path();
    _PushFile(src_);
  } else
    assert(0);

  _zip_file.close();
  return true;
}

bool XyzipImp::Unzip(const char *dest, const char *src) {
  path src_(src);
  path dest_(dest);

  if (!is_regular_file(src_))
    return false;

  if (!is_directory(dest_) && !create_directory(dest_))
    return false;

  bool ret = true;
  _unzip_dir_dest = dest_;

  try {
    _unzip_file.open(src_, std::ios::in | std::ios::binary);
    while (_PopFile())
      ;
  } catch (const std::exception &ex) {
    std::cout << ex.what() << std::endl;
    ret = false;
  }

  _unzip_file.close();
  return ret;
}

void XyzipImp::SetKey(unsigned key) {
  _key = key;
  _GenerateLevel();
}

void XyzipImp::_PushFile(const path &src) {
  assert(is_regular_file(src));
  assert(_zip_file.is_open());

  if (src == _zip_file_dest)
    return;

  FileHead file_h;
  file_h.file_len = directory_entry(src).file_size();

  std::string path_str = src.string().substr(_zip_root.string().length());
  file_h.path_len = (unsigned)path_str.length();

  _EncodeWrite(_zip_file, &CHAR_CAST(file_h), sizeof(file_h));
  _EncodeWrite(_zip_file, path_str.c_str(), file_h.path_len);

  std::ifstream fin(src, std::ios::in | std::ios::binary);
  _Compress(_zip_file, fin);

  _zip_file.flush();
}

void XyzipImp::_PushDirectory(const path &src) {
  assert(is_directory(src));
  assert(_zip_file.is_open());

  for (auto &path_entry : directory_iterator(src)) {
    if (path_entry.is_directory())
      _PushDirectory(path_entry);
    else if (path_entry.is_regular_file())
      _PushFile(path_entry);
    else
      assert(0);
  }
}

bool XyzipImp::_PopFile() {
  if (_unzip_file.peek() == EOF)
    return false;

  FileHead file_h;
  _DecodeRead(_unzip_file, &CHAR_CAST(file_h), sizeof(file_h));

  if (file_h.tag != kFileTag)
    throw std::exception();

  char buff[1024]{};
  _DecodeRead(_unzip_file, buff, file_h.path_len);
  path path_ = _unzip_dir_dest.wstring() + path(buff).wstring();

  if (!exists(path_.parent_path()))
    create_directories(path_.parent_path());

  std::ofstream fout(path_, std::ios::out | std::ios::binary);
  _Decompress(fout, _unzip_file, file_h);

  return true;
}

void XyzipImp::_Compress(std::ofstream &fout, std::ifstream &fin) const {
  assert(fin.is_open() && fout.is_open());
  auto write_rle = [this](std::ofstream &fout, const RleHead &rle_h) {
    if (rle_h.count < 3) {
      for (unsigned i = 0; i < rle_h.count; ++i)
        _EncodeWrite(fout, &CHAR_CAST(rle_h.data));
      return;
    }

    _EncodeWrite(fout, &CHAR_CAST(rle_h.tag));
    _EncodeWrite(fout, &CHAR_CAST(rle_h.count));
    _EncodeWrite(fout, &CHAR_CAST(rle_h.data));
  };

  RleHead rle_h;
  unsigned input = 0;

  for (;; ++rle_h.count, rle_h.data = input) {
    fin.read(&CHAR_CAST(input), kStep);
    if (fin.eof()) {
      write_rle(fout, rle_h);
      _EncodeWrite(fout, &CHAR_CAST(input), fin.gcount());
      break;
    }

    if (input == rle_h.data || rle_h.count == 0)
      continue;

    write_rle(fout, rle_h);
    rle_h.count = 0;
  }
}

void XyzipImp::_Decompress(std::ofstream &fout, std::ifstream &fin,
                           FileHead &file_h) const {
  assert(fin.is_open() && fout.is_open());
  RleHead rle_h;
  unsigned input = 0;
  auto left = file_h.file_len;

  while (left >= kStep) {
    _DecodeRead(fin, &CHAR_CAST(input));

    if (input != kRleTag) {
      fout.write(&CHAR_CAST(input), kStep);
      left -= fin.gcount();
      continue;
    }

    _DecodeRead(fin, &CHAR_CAST(rle_h.count));
    _DecodeRead(fin, &CHAR_CAST(rle_h.data));

    for (unsigned i = 0; i < rle_h.count; ++i)
      fout.write(&CHAR_CAST(rle_h.data), kStep);

    left -= (decltype(left))kStep * rle_h.count;
  }

  _DecodeRead(fin, &CHAR_CAST(input), left);
  fout.write(&CHAR_CAST(input), left);
}

void XyzipImp::_EncodeWrite(std::ofstream &fout, const char *str,
                            std::streamsize count) const {
  unsigned idx = 0;
  unsigned code = 0;

  for (; idx < count; idx += kStep) {
    code = Encrypt(UINT_CAST(str[idx]), _key, _level);
    fout.write(&CHAR_CAST(code), std::min((std::streamsize)kStep, count - idx));
  }
}

void XyzipImp::_DecodeRead(std::ifstream &fin, char *str,
                           std::streamsize count) const {
  unsigned idx = 0;
  unsigned code = 0;
  unsigned len = 0;

  for (; idx < count; idx += kStep) {
    len = std::min((unsigned)kStep, (unsigned)count - idx);
    fin.read(&CHAR_CAST(code), len);
    auto temp = Decrypt(code, _key, _level);
    memcpy(&str[idx], &temp, len);
  }
}

void XyzipImp::_GenerateLevel() {
  auto key = _key;
  _level = 0;

  while (key) {
    _level += key & 0x1;
    key >>= 1;
  }
}
