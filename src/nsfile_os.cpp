/*!
\file nsfileos.cpp

\brief Functions for OS file access

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/

#include <iostream>
#include <nsfile_os.h>
#include <nsres_manager.h>
#ifdef WIN32
#include <filesystem>
#include <Windows.h>
#else
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#endif

#include <fstream>
#include <string>
#include <iostream>
#include <iterator>

namespace nsfile_os
{

#ifdef WIN32

bool file_exists(const nsstring & filename)
{
	std::tr2::sys::path p;
	p = filename;
	return std::tr2::sys::exists(p);
}

bool dir_exists(const nsstring & filename)
{
	
	std::tr2::sys::path p;
	p = filename;
	return std::tr2::sys::exists(p);
}

bool create_dir(const nsstring & path)
{
	bool ret = false;
	nsstring pth(path);
	nsstring dr;
	size_t up = pth.find_first_of("/\\")+1;
	std::tr2::sys::path p;
	while (up != 0)
	{
		dr += pth.substr(0, up);
		pth = pth.substr(up);
		up = pth.find_first_of("/\\")+1;
		p = dr;
		try
		{
			ret = std::tr2::sys::create_directory(p) || ret;
		}
		catch (std::exception &)
		{
			// do nothing
		}
	}
	return ret;
}

uint32 remove_dir(const nsstring & dirpath)
{
	std::tr2::sys::path p;
	p = dirpath;
	return std::tr2::sys::remove_all(p);
}

bool remove_file(const nsstring & filename)
{
	std::tr2::sys::path p;
	p = filename;
	return std::tr2::sys::remove(p);
}

bool rename_dir(const nsstring & oldpath, const nsstring & newpath)
{
	std::tr2::sys::path p1,p2;
	p1 = oldpath; p2 = newpath;
	return std::tr2::sys::rename(p1, p2);
}

bool rename_file(const nsstring & oldname, const nsstring & newname)
{
	std::tr2::sys::path p1, p2;
	p1 = oldname; p2 = newname;
	return std::tr2::sys::rename(p1, p2);
}

nsstring cwd()
{
	char buffer[MAX_PATH];
	GetModuleFileName(NULL, buffer, MAX_PATH);
	nsstring::size_type pos = nsstring( buffer ).find_last_of( "\\/" );
    return nsstring( buffer ).substr( 0, pos);
}

#else

bool file_exists(const nsstring & filename)
{
	struct stat buffer;   
	return (stat(filename.c_str(), &buffer) == 0);
}

bool dir_exists(const nsstring & filename)
{
	struct stat buffer;   
	return (stat(filename.c_str(), &buffer) == 0 && S_ISDIR(buffer.st_mode));
}

bool create_dir(const nsstring & path)
{
	char tmp[256];
	char *p = NULL;
	size_t len;
	nsstring stripped_path = nsres_manager::path_from_filename(path);

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

bool remove_file(const nsstring & filename)
{
	return (remove(filename.c_str()) == 0);
}

bool rename_dir(const nsstring & oldpath, const nsstring & newpath)
{
	return (rename(oldpath.c_str(), newpath.c_str()) == 0);
}

bool rename_file(const nsstring & oldname, const nsstring & newname)
{
	return (rename(oldname.c_str(), newname.c_str()) == 0);
}

nsstring cwd()
{
	char temp[256];
	uint32 sz = readlink("/proc/self/exe", temp, 256);
	nsstring ret(temp);
	ret.resize(sz);
	return nsres_manager::path_from_filename(ret);
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

}
