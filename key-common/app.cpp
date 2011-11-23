#include "app.h"

#include <key-common/platform.hpp>

#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

using namespace std;

char * app_path = NULL;

void setAppPath(char p[])
{
	app_path = &p[0];
}

string getAppPath()
{
	if (app_path == NULL)
		return "";
	string s(app_path);
	if (s.find(PATH_SEPARATOR) != string::npos)
	{
		return s.substr(0, s.rfind(PATH_SEPARATOR));
	}
	else
	{
		return "";
	}
	return string(app_path);
}

static string joinNormalizedPath(string root, string path)
{
	while (path.size() > 0)
	{
		size_t partend = path.find(PATH_SEPARATOR);
		string part;
		bool nosep = false;
		if (partend == path.npos)
		{
			part = path;
			path = "";
			nosep = true;
		}
		else
		{
			part = path.substr(0, partend);
			path = path.substr(partend + 1);
		}
		if (part == "..")
		{
			size_t rpos = root.rfind(PATH_SEPARATOR, root.size() - 2);
			if (rpos == root.npos)
			{
				if (nosep)
					root = root + part;
				else
					root = root + part + PATH_SEPARATOR;
			}
			else
			{
				string rootpart = root.substr(rpos + 1, root.size() - 2 - rpos);
				if (rootpart == "..")
				{
					if (nosep)
						root = root + part;
					else
						root = root + part + PATH_SEPARATOR;
				}
				else
				{
					root = root.substr(0, rpos + 1);
				}
			}
		}
		else if (part != "." && part != "")
		{
			if (nosep)
				root = root + part;
			else
				root = root + part + PATH_SEPARATOR;
		}
	}
	return root;
}

string joinPath(const string& root, const string& file)
{
	string path = unixPathToPlatformPath(file);

	if (root.size() == 0)
		return path;
	if (file.size() == 0)
		return root;
	if (root[root.size() - 1] == PATH_SEPARATOR)
	{
		if (path[0] == PATH_SEPARATOR)
			return joinNormalizedPath(root, path.substr(1));
		else
			return joinNormalizedPath(root, path);
	}
	else
	{
		if (path[0] == PATH_SEPARATOR)
			return joinNormalizedPath(root + PATH_SEPARATOR, path.substr(1));
		else
			return joinNormalizedPath(root + PATH_SEPARATOR, path);
	}
}

void search_and_replace(std::string& value, std::string const& search,std::string const& replace)
{
    std::string::size_type  next;

    for(next = value.find(search);        // Try and find the first match
        next != std::string::npos;        // next is npos if nothing was found
        next = value.find(search,next)    // search for the next match starting after
                                          // the last match that was found.
       )
    {
        // Inside the loop. So we found a match.
        value.replace(next,search.length(),replace);   // Do the replacement.
        next += replace.length();                      // Move to just after the replace
                                                       // This is the point were we start
                                                       // the next search from. 
    }
}

string fullPathTo(const string& file)
{
	string p = getAppPath();
	if (p == "")
		return file;
#if PATH_SEPARATOR == '\\'
	string winfile = file;
	search_and_replace(winfile, "/", "\\");
	return joinPath(p, winfile);
#else
	return joinPath(p, file);
#endif
}

string rootDirPathTo(const string& root, const string& file)
{
	if (root == "")
		return file;
#if PATH_SEPARATOR == '\\'
	string winfile = file;
	search_and_replace(winfile, "/", "\\");
	return joinPath(root, winfile);
#else
	return joinPath(root, file);
#endif
}

bool stringFromFile(std::string & output, const string& filename) {
	ifstream ifs(filename);
	if (!ifs)
		return false;

	ifs.seekg(0, std::ios::end);
	output.reserve(ifs.tellg());
	ifs.seekg(0, std::ios::beg);

	output.assign((std::istreambuf_iterator<char>(ifs)),
				std::istreambuf_iterator<char>());

	return true;
}

#if PATH_SEPARATOR != '/'
string unixPathToPlatformPath(const string& path)
{
	ostringstream ss;

	size_t pos = path.find('/');
	size_t start = 0;
	while (pos != path.npos)
	{
		ss << path.substr(start, pos - start) << PATH_SEPARATOR;
		start = pos + 1;
		if (start >= path.size())
			pos = path.npos;
		else
			pos = path.find('/', start);
	}
	if (start < path.size())
		ss << path.substr(start);

	return ss.str();
}
#endif

void critical_message(const std::string title, const std::string text)
{
#if defined(WIN32) || defined(_WINDOWS)
    MessageBoxA(0, text.c_str(), title.c_str(),
        MB_OK | MB_SETFOREGROUND | MB_ICONSTOP);
#else
    std::cout << "Critical error: " << title << std::endl << text << std::endl;
#endif
}

void terminate_app() {
	exit(1);
}