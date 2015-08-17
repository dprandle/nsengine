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
	enum WriteMode {APPEND,OVERWRITE};

	// Ctors
	NSLogFile(const nsstring & text="", const nsstring & fileName="enginedebug.log", const nsstring & directory="logs");
	NSLogFile(const nsstringstream & stream, const nsstring & fileName="enginedebug.log", const nsstring & directory="logs");

	// Dtor
	~NSLogFile();

	void clear();

	// set
	void setFileName(const nsstring & fileName);
	void setDirectory(const nsstring & directory);
	void setWriteMode(WriteMode mode);
	void setTimeStampEnabled(bool);

	// get
	const nsstring & getFileName() const;
	const nsstring & getDirectory() const;
	WriteMode getWriteMode() const;
	bool isTimeStampEnabled() const;

	// functionality
	bool write(const nsstring & text);
	bool write(const nsstringstream & num);
	bool write(float num);
	bool write(int32 num);

	bool writeDate();

	// Static methods
	static bool writeDate(const nsstring & fname,WriteMode mode = OVERWRITE);
	static bool writeToCom(const nsstring & text,WriteMode mode = APPEND);
	static bool writeTo(const nsstring & text, const nsstring &fname, WriteMode = APPEND);

private:
	WriteMode currentWriteMode;
	nsstring filename;
	nsstring dir;
	bool timeStampEnabled;
};

#endif  // NSLogFile_H
