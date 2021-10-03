#pragma once

#ifdef XYZIP_EXPORTS
#define XYZIP_AIP __declspec(dllexport)
#else
#define XYZIP_AIP __declspec(dllimport)
#endif

#ifdef __cplusplus
extern "C"
{
#endif

	XYZIP_AIP bool xy_zip(const char* path, const char* directory);
	XYZIP_AIP bool xy_unzip(const char* file, const char* directory);

#ifdef __cplusplus
}
#endif