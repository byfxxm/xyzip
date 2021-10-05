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
	xy_setk('nice');
	xy_zip("F:\\VSProject\\tests\\freegate", "F:\\VSProject\\tests\\pack");
	xy_unzip("F:\\VSProject\\tests\\pack\\freegate.xyzip", "F:\\VSProject\\tests\\pack");
	std::cout << "Hello World!\n";
}