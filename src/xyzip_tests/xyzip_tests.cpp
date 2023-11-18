// xyzip_tests.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "../xyzip/xyzip.hpp"
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <string>

xyzip::Xyzip g_xyzip;

int main() {
#if defined(_DEBUG) || !defined(NDEBUG)
  std::filesystem::remove_all("tests\\pack");
  g_xyzip.SetKey(0x2341);

  if (!g_xyzip.Zip("tests", "tests\\pics"))
    return -1;

  std::cout << "zipped" << std::endl;

  std::filesystem::remove_all("tests\\unzip");
  if (!g_xyzip.Unzip("tests\\unzip", "tests\\pics.xyzip"))
    return -1;

  std::cout << "unzipped" << std::endl;
#else
  std::filesystem::path src, dest;
  std::string key;

  while (1) {
    std::cout << "(1)zip or (2)unzip: ";
    std::cin >> src;

    try {
      if (std::stoi(src) == 1) {
        do {
          std::cout << "input source directory: ";
          std::cin >> src;
        } while (!std::filesystem::is_directory(src) &&
                 !std::filesystem::is_regular_file(src));

        dest = src.parent_path();
        std::cout << "input key: ";
        std::cin >> key;
        if (!key.empty())
          g_xyzip.SetKey(std::stoul(key));

        auto res = g_xyzip.Zip(dest.string().c_str(), src.string().c_str());
        std::cout << (res ? "zip success!" : "zip fail!") << std::endl;
      } else if (std::stoi(src) == 2) {
        do {
          std::cout << "input source file: ";
          std::cin >> src;
        } while (!std::filesystem::is_regular_file(src));

        dest = src.parent_path();
        std::cout << "input key: ";
        std::cin >> key;
        if (!key.empty())
          g_xyzip.SetKey(std::stoul(key));

        auto res = g_xyzip.Unzip(dest.string().c_str(), src.string().c_str());
        std::cout << (res ? "unzip success!" : "unzip fail!") << std::endl;
      }
    } catch (...) {
      std::cout << "input error!" << std::endl;
    }
  }
#endif

  return 0;
}
