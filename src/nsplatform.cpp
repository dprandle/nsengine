/*!
\file nsfileos.cpp

\brief Functions for OS file access

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/

#include <iostream>
#include <nsplatform.h>
#include <nsasset_manager.h>
#ifdef WIN32
#include <filesystem>
#include <Windows.h>
#elif __APPLE__
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <time.h>
#include <libproc.h>
#else
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <time.h>
#endif

#include <nsasset_manager.h>
#include <fstream>
#include <string>
#include <iostream>
#include <iterator>
#include <stdio.h>

namespace platform
{

#ifdef WIN32

uint64 freq;

void platform_init()
{
	LARGE_INTEGER pf;
	freq = QueryPerformanceFrequency(&pf);
	freq = pf.QuadPart;
}

bool file_exists(const nsstring & filename)
{
    std::ifstream ifile(filename);
    if (ifile.is_open())
    {
        ifile.close();
        return true;
    }
    return false;
}

bool path_exists(nsstring path)
{
    DWORD dwAttrib = GetFileAttributes(windows_path(path).c_str());
    return (dwAttrib != INVALID_FILE_ATTRIBUTES &&
           (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

bool create_dir(const nsstring & path)
{
	bool ret = false;
    nsstring pth(path);
	nsstring dr;
	size_t up = pth.find_first_of("/\\")+1;
    nsstring p;
	while (up != 0)
	{
		dr += pth.substr(0, up);
		pth = pth.substr(up);
		up = pth.find_first_of("/\\")+1;
        p = dr;
        CreateDirectory(p.c_str(),NULL);
	}
	return ret;
}

uint32 remove_dir(const nsstring & dirpath)
{
	std::tr2::sys::path p;
	p = dirpath;
	return std::tr2::sys::remove_all(p);
}

bool remove(const nsstring & filename)
{
    return (::remove(filename.c_str()) == 0);
}

bool rename(const nsstring & oldname, const nsstring & newname)
{
    return (::rename(oldname.c_str(), newname.c_str()) == 0);
}

double system_time()
{
	LARGE_INTEGER cnt;
	QueryPerformanceCounter(&cnt);
	return double(cnt.QuadPart) / double(freq);
}

nsstring cwd()
{
	char buffer[MAX_PATH];
	GetModuleFileName(NULL, buffer, MAX_PATH);
    nsstring str(buffer);
    nsstring::size_type pos = str.find_last_of( "\\/" );
    str = str.substr(0, pos) + "/";
    return str;
}

#elif __APPLE__

bool file_exists(const nsstring & filename)
{
	struct stat buffer;   
	return (stat(filename.c_str(), &buffer) == 0);
}

bool path_exists(nsstring filename)
{
	struct stat buffer;   
	return (stat(filename.c_str(), &buffer) == 0 && S_ISDIR(buffer.st_mode));
}

bool create_dir(const nsstring & path)
{
	char tmp[256];
	char *p = NULL;
	size_t len;
	nsstring stripped_path = nsasset_manager::path_from_filename(path);

	snprintf(tmp, sizeof(tmp),"%s",stripped_path.c_str());
	len = strlen(tmp);
	if(tmp[len - 1] == '/')
		tmp[len - 1] = 0;
	for(p = tmp + 1; *p; p++)
		if(*p == '/') {
			*p = 0;
			mkdir(tmp, S_IRWXU);
			*p = '/';
		}
	return (mkdir(tmp, S_IRWXU) == 0);
}

double system_time()
{
	timeval ts;
	gettimeofday(&ts, nullptr);
	return (double)ts.tv_sec + (double)ts.tv_usec / 1000000.0;
}

uint32 remove_dir(const nsstring & dirpath)
{
    DIR *dir;
    struct dirent *entry;
    char path[PATH_MAX];
	
    dir = opendir(dirpath.c_str());
	
    if (dir == NULL)
        return 0;

    while ((entry = readdir(dir)) != NULL)
	{
        if (strcmp(entry->d_name, ".") && strcmp(entry->d_name, ".."))
		{
            snprintf(path, (size_t) PATH_MAX, "%s/%s", dirpath.c_str(), entry->d_name);

            if (entry->d_type == DT_DIR)
                remove_dir(path);
			remove(path);
        }

    }
    closedir(dir);
	rmdir(dirpath.c_str());
    return 1;
}

bool remove(const nsstring & filename)
{
	return (::remove(filename.c_str()) == 0);
}

bool rename(const nsstring & oldname, const nsstring & newname)
{
	return (::rename(oldname.c_str(), newname.c_str()) == 0);
}

nsstring cwd()
{
    pid_t pid;
    char pathbuf[1024];
    pid = getpid();
    proc_pidpath(pid, pathbuf, sizeof(pathbuf));
    nsstring ret(pathbuf);
    return nsasset_manager::path_from_filename(ret);
}

void platform_init()
{
	
}

#else

bool file_exists(const nsstring & filename)
{
	struct stat buffer;   
	return (stat(filename.c_str(), &buffer) == 0);
}

bool path_exists(nsstring filename)
{
	struct stat buffer;   
	return (stat(filename.c_str(), &buffer) == 0 && S_ISDIR(buffer.st_mode));
}

bool create_dir(const nsstring & path)
{
	char tmp[256];
	char *p = NULL;
	size_t len;
	nsstring stripped_path = nsasset_manager::path_from_filename(path);

	snprintf(tmp, sizeof(tmp),"%s",stripped_path.c_str());
	len = strlen(tmp);
	if(tmp[len - 1] == '/')
		tmp[len - 1] = 0;
	for(p = tmp + 1; *p; p++)
		if(*p == '/') {
			*p = 0;
			mkdir(tmp, S_IRWXU);
			*p = '/';
		}
	return (mkdir(tmp, S_IRWXU) == 0);
}

double system_time()
{
	timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return (double)ts.tv_sec + (double)ts.tv_nsec / 1000000000.0;
}

uint32 remove_dir(const nsstring & dirpath)
{
    DIR *dir;
    struct dirent *entry;
    char path[PATH_MAX];

    if (path == NULL)
        return 0;
	
    dir = opendir(dirpath.c_str());
	
    if (dir == NULL)
        return 0;

    while ((entry = readdir(dir)) != NULL)
	{
        if (strcmp(entry->d_name, ".") && strcmp(entry->d_name, ".."))
		{
            snprintf(path, (size_t) PATH_MAX, "%s/%s", dirpath.c_str(), entry->d_name);

            if (entry->d_type == DT_DIR)
                remove_dir(path);
			remove(path);
        }

    }
    closedir(dir);
	rmdir(dirpath.c_str());
    return 1;
}

bool remove(const nsstring & filename)
{
	return (::remove(filename.c_str()) == 0);
}

bool rename(const nsstring & oldname, const nsstring & newname)
{
	return (::rename(oldname.c_str(), newname.c_str()) == 0);
}

nsstring cwd()
{
	char temp[256];
	uint32 sz = readlink("/proc/self/exe", temp, 256);
	nsstring ret(temp);
	ret.resize(sz);
	return nsasset_manager::path_from_filename(ret);
}

void platform_init()
{
	
}

#endif

void read(const nsstring & fname, ui8_vector * contents)
{
	// open the file:
    std::ifstream file(fname, std::ios::binary);

	if (!file.is_open())
		return;

    // Stop eating new lines in binary mode!!!
    file.unsetf(std::ios::skipws);

    // get its size:
    std::streampos fileSize;

    file.seekg(0, std::ios::end);
    fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    // reserve capacity
    contents->reserve(static_cast<ui8_vector::size_type>(fileSize));

    // read the data:
    contents->insert(contents->begin(),
               std::istream_iterator<char>(file),
               std::istream_iterator<char>());
}

nsstring & windows_path(nsstring & convert)
{
    for (uint32 i = 0; i < convert.size(); ++i)
    {
        if (convert[i] == '/')
            convert[i] = '\\';
    }
    return convert;
}


nsstring & unix_path(nsstring & convert)
{
    for (uint32 i = 0; i < convert.size(); ++i)
    {
        if (convert[i] == '\\')
            convert[i] = '/';
    }
    return convert;
}

}
