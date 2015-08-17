#ifndef NSFILEOS_H
#define NSFILEOS_H

#include <nsglobal.h>
#include <sys/stat.h>

namespace nsfileio
{

bool file_exists(const nsstring & filename);
bool dir_exists(const nsstring & filename);

bool create_dir(const nsstring & path);

uint32 remove_dir(const nsstring & path);
bool remove_file(const nsstring & filename);

bool rename_dir(const nsstring & oldpath, const nsstring & newpath);
bool rename_file(const nsstring & oldname, const nsstring & newname);

nsstring cwd();

void read(const nsstring & fname, nschararray * contents);

}


#endif
