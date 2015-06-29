/*!
\file nsfileos.cpp

\brief Functions for OS file access

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/

#include <nsfileos.h>
#include <filesystem>
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
			ret = std::tr2::sys::create_directory(p) || ret;
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
}