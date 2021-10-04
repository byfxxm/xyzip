// xyzip_tests.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "../xyzip/xyzip.h"

#ifdef _DEBUG
#pragma comment(lib, "../Debug/xyzip.lib")
#else
#pragma comment(lib, "../Release/xyzip.lib")
#endif

int main()
{
	xy_zip("F:\\VSProject\\xyzip\\src\\xyzip_tests\\test_files\\freegate", "F:\\VSProject\\xyzip\\src\\xyzip_tests\\test_files\\pack");
	xy_unzip("F:\\VSProject\\xyzip\\src\\xyzip_tests\\test_files\\pack\\freegate.xyzip", "F:\\VSProject\\xyzip\\src\\xyzip_tests\\test_files\\pack");
	std::cout << "Hello World!\n";
}