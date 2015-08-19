/*!
\file nslog_file.h

\brief Header file for NSLogFile class

This file contains all of the neccessary declarations for the NSLogFile class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/

#ifndef NSLOGFILE_H
#define NSLOGFILE_H

// Inlcudes
#include <nsglobal.h>
#include <string>
#include <sstream>

// Class Forward Declarations

class NSLogFile
{
public:

	enum write_m {
		wm_append,
		wm_overwrite
	};

	NSLogFile(const nsstring & text="", const nsstring & fileName="enginedebug.log", const nsstring & directory="logs");
	NSLogFile(const nsstringstream & stream, const nsstring & fileName="enginedebug.log", const nsstring & directory="logs");

	~NSLogFile();

	void clear();

	void set_file_name(const nsstring & fileName);

	void set_dir(const nsstring & directory);

	void set_write_mode(write_m mode);

	void enable_timestamp(bool enable_);

	const nsstring & file_name() const;

	const nsstring & dir() const;

	write_m write_mode() const;

	bool timestamp() const;

	bool write(const nsstring & text);

	bool write(const nsstringstream & num);

	bool write(float num);

	bool write(int32 num);

	bool write_date();

	static bool write_date(const nsstring & fname,write_m mode = wm_overwrite);

	static bool write_to_com(const nsstring & text,write_m mode = wm_append);

	static bool write_to(const nsstring & text, const nsstring &fname, write_m = wm_append);

private:
	write_m m_current_wm;
	nsstring m_fname;
	nsstring m_dir;
	bool m_ts_enabled;
};

#endif  // NSLogFile_H
