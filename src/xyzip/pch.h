// pch.h: 这是预编译标头文件。
// 下方列出的文件仅编译一次，提高了将来生成的生成性能。
// 这还将影响 IntelliSense 性能，包括代码完成和许多代码浏览功能。
// 但是，如果此处列出的文件中的任何一个在生成之间有更新，它们全部都将被重新编译。
// 请勿在此处添加要频繁更新的文件，这将使得性能优势无效。

#pragma once

// 添加要在此处预编译的标头
#include "framework.h"
#include <filesystem>
#include <iostream>
#include <fstream>
#include <cassert>
#include <functional>
#include <unordered_map>

#define CHAR_CAST(var) (*(char*)&(var))
#define UINT_CAST(var) (*(unsigned*)&(var))

constexpr auto EXT = L".xyzip";
constexpr auto FILE_TAG = 0xFABCBCDC;
constexpr auto RLE_TAG = 0xFFABCBCD;
constexpr auto STEP = 4;

namespace xyzip {}
using namespace xyzip;
using namespace std::filesystem;