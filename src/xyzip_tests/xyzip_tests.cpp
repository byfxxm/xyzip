// xyzip_tests.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "../xyzip/xyzip.hpp"
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <string>

#ifdef _WIN64
#define PLATFORM "x64"
#else
#define PLATFORM ""
#endif
#ifdef _DEBUG
#define CONFIGURATION "Debug"
#else
#define CONFIGURATION "Release"
#endif
#define LIB_DIR "../" PLATFORM "/" CONFIGURATION "/"

#pragma comment(lib, LIB_DIR "xyzip.lib")

int main() {
#if defined(_DEBUG) || !defined(NDEBUG)
  std::filesystem::remove_all("tests\\pack");
  xy_setk(0x2341);

  if (!xy_zip("tests", "tests\\pics"))
    return -1;

  std::cout << "zipped" << std::endl;

  std::filesystem::remove_all("tests\\unzip");
  if (!xy_unzip("tests\\unzip", "tests\\pics.xyzip"))
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
          xy_setk(std::stoul(key));

        auto res = xy_zip(dest.string().c_str(), src.string().c_str());
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
          xy_setk(std::stoul(key));

        auto res = xy_unzip(dest.string().c_str(), src.string().c_str());
        std::cout << (res ? "unzip success!" : "unzip fail!") << std::endl;
      }
    } catch (...) {
      std::cout << "input error!" << std::endl;
    }
  }
#endif

  return 0;
}
