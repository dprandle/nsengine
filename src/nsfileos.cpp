/*!
\file nsfileos.cpp

\brief Functions for OS file access

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/

#include <nsfileos.h>
#include <filesystem>
#include <Windows.h>
#include <string>
#include <iostream>

namespace nsfileio
{
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

nsuint remove_dir(const nsstring & dirpath)
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

void read(const nsstring & fname, nschararray * contents)
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
    contents->reserve(fileSize);

    // read the data:
    contents->insert(contents->begin(),
               std::istream_iterator<nschar>(file),
               std::istream_iterator<nschar>());
}

}
