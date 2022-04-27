#pragma once

#ifdef XYZIP_EXPORTS
#define XYZIP_AIP __declspec(dllexport)
#else
#define XYZIP_AIP __declspec(dllimport)
#endif

extern "C" {
	XYZIP_AIP bool xy_zip(const char* dest, const char* src);
	XYZIP_AIP bool xy_unzip(const char* dest, const char* src);
	XYZIP_AIP void xy_setk(unsigned key);
}