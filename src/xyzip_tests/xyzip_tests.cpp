// xyzip_tests.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <filesystem>
#include <cstdlib>
#include <string>
#include "../xyzip/xyzip.h"

#ifdef _WIN64
#define PLATFORM "x64"
#else
#define PLATFORM ""
#endif

#ifdef _DEBUG
#define CONFIG "Debug"
#else
#define CONFIG "Release"
#endif

#pragma comment(lib, "../" PLATFORM "/" CONFIG "/xyzip.lib")

int main()
{
#if 1
	std::filesystem::remove_all("tests\\pack");
	xy_setk('nike');

	if (!xy_zip("tests", "tests\\pics"))
		return -1;

	std::cout << "zipped" << std::endl;

	std::filesystem::remove_all("tests\\unzip");
	if (!xy_unzip("tests\\unzip", "tests\\pics.xyzip"))
		return -1;

	std::cout << "unzipped" << std::endl;
#else
	std::string s1, s2, s3;

	while (1)
	{
		std::cout << "(1)zip or (2)unzip: ";
		std::cin >> s1;

		try
		{
			if (std::stoi(s1) == 1)
			{
				do
				{
					std::cout << "input source directory: ";
					std::cin >> s1;
				} while (!std::filesystem::is_directory(s1) && !std::filesystem::is_regular_file(s1));

				do
				{
					std::cout << "input dest directory: ";
					std::cin >> s2;
				} while (!std::filesystem::is_directory(s2));

				std::cout << "input key: ";
				std::cin >> s3;
				if (!s3.empty())
					xy_setk(*(unsigned*)s3.c_str());

				auto res = xy_zip(s2.c_str(), s1.c_str());
				std::cout << (res ? "zip success!" : "zip fail!") << std::endl;
			}
			else if (std::stoi(s1) == 2)
			{
				do
				{
					std::cout << "input source file: ";
					std::cin >> s1;
				} while (!std::filesystem::is_regular_file(s1));

				do
				{
					std::cout << "input dest directory: ";
					std::cin >> s2;
				} while (!std::filesystem::is_directory(s2));

				std::cout << "input key: ";
				std::cin >> s3;
				if (!s3.empty())
					xy_setk(*(unsigned*)s3.c_str());

				auto res = xy_unzip(s2.c_str(), s1.c_str());
				std::cout << (res ? "unzip success!" : "unzip fail!") << std::endl;
			}
		}
		catch (...)
		{
			std::cout << "input error!" << std::endl;
		}
	}
#endif

	return 0;
}