#pragma once

#include <key-common/lib_key_common.h>
#include <string>

#ifdef WIN32
#define PATH_SEPARATOR '\\'
#else
#define PATH_SEPARATOR '/'
#endif

LIB_KEY_COMMON void setAppPath(char p[]);
LIB_KEY_COMMON std::string fullPathTo(const std::string& file);
LIB_KEY_COMMON std::string rootDirPathTo(const std::string& root, const std::string& file);
LIB_KEY_COMMON std::string getAppPath();
/* append relative path to the root path */
LIB_KEY_COMMON std::string joinPath(const std::string& root, const std::string& file);
#if PATH_SEPARATOR == '/'
	/* convert path separated with "/" to correct path on this platform */
	inline std::string unixPathToPlatformPath(const std::string& path)
	{
		return path;
	}
#else
    /* convert path separated with "/" to correct path on this platform */
	LIB_KEY_COMMON std::string unixPathToPlatformPath(const std::string& path);
#endif

LIB_KEY_COMMON bool stringFromFile(std::string & output, const std::string& filename);

LIB_KEY_COMMON void critical_message(const std::string title, const std::string text);
LIB_KEY_COMMON void terminate_app();