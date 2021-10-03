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
    zip("F:\\VSProject\\xyzip\\src\\test_rc");
    std::cout << "Hello World!\n";
}