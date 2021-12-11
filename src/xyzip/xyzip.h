#pragma once
#include <stdint.h>

#ifdef XYZIP_EXPORTS
#define XYZIP_AIP __declspec(dllexport)
#else
#define XYZIP_AIP __declspec(dllimport)
#endif

#ifdef __cplusplus
extern "C"
{
#endif
	XYZIP_AIP bool xy_zip(const char* dest, const char* src);
	XYZIP_AIP bool xy_unzip(const char* dest, const char* src);
	XYZIP_AIP void xy_setk(uint32_t key);
#ifdef __cplusplus
}
#endif