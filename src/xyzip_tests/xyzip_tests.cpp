// xyzip_tests.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <filesystem>
#include <cstdlib>
#include <string>
#include "../xyzip/xyzip.h"

#ifdef _DEBUG
#pragma comment(lib, "../Debug/xyzip.lib")
#else
#pragma comment(lib, "../Release/xyzip.lib")
#endif

using namespace std;

int main()
{
	string s1, s2, s3;

	while (1)
	{
		cout << "(1)zip or (2)unzip: ";
		cin >> s1;

		try
		{
			if (std::stoi(s1) == 1)
			{
				do
				{
					cout << "input source directory: ";
					cin >> s1;
				} while (!filesystem::is_directory(s1) && !filesystem::is_regular_file(s1));

				do
				{
					cout << "input dest directory: ";
					cin >> s2;
				} while (!filesystem::is_directory(s2));

				cout << "input key: ";
				cin >> s3;
				if (!s3.empty())
					xy_setk(*(unsigned*)s3.c_str());

				auto res = xy_zip(s1.c_str(), s2.c_str());
				cout << (res ? "zip success!" : "zip fail!") << endl;
			}
			else if (std::stoi(s1) == 2)
			{
				do
				{
					cout << "input source file: ";
					cin >> s1;
				} while (!filesystem::is_regular_file(s1));

				do
				{
					cout << "input dest directory: ";
					cin >> s2;
				} while (!filesystem::is_directory(s2));

				cout << "input key: ";
				cin >> s3;
				if (!s3.empty())
					xy_setk(*(unsigned*)s3.c_str());

				auto res = xy_unzip(s1.c_str(), s2.c_str());
				cout << (res ? "unzip success!" : "unzip fail!") << endl;
			}
		}
		catch (...)
		{
			cout << "input error!" << endl;
		}
	}

	return 0;
}