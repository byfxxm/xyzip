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

	XYZIP_AIP bool xy_zip(const char* path);
	XYZIP_AIP bool xy_unzip(const char* path);

#ifdef __cplusplus
}
#endif