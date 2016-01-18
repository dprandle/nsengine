#ifndef NSFILEOS_H
#define NSFILEOS_H

#include <sys/stat.h>
#include <nsvector.h>

namespace nsfile_os
{

void platform_init();

bool file_exists(const nsstring & filename);

bool path_exists(const nsstring path);

bool create_dir(const nsstring & path);

uint32 remove_dir(const nsstring & path);

bool remove(const nsstring & filename);

bool rename(const nsstring & oldname, const nsstring & newname);

nsstring cwd();

void read(const nsstring & fname, ui8_vector * contents);

double system_time();

nsstring & windows_path(nsstring & convert);
nsstring & unix_path(nsstring & convert);

}


#endif
