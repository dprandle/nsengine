 /*!
\file nslogfile.cpp

\brief Definition file for NSLogFile class

This file contains all of the neccessary definitions for the NSLogFile class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/

// Includes
#include <nsglobal.h>
#include <nslogfile.h>
#include <ctime>
#include <iomanip>
#include <chrono>


/* Constructor 1
Description:
	This constructor allows direct writing to a logfile - no need to keep track of an instance
	To write to current dir use QString() as directory

Parameters:
	text : Text to write to the logfile
	fileName : the name of the logfile to write to
	directory : the directory where the logfile is stored
*/
NSLogFile::NSLogFile(const nsstring & text, const nsstring & fileName, const nsstring & directory):
currentWriteMode(APPEND),
filename(fileName),
dir(directory),
timeStampEnabled(false)
{
	if (text != "")
		write(text);
}

NSLogFile::NSLogFile(const nsstringstream & stream, const nsstring & fileName, const nsstring & directory):
currentWriteMode(APPEND),
filename(fileName),
dir(directory),
timeStampEnabled(false)
{
	write(stream);
}

NSLogFile::~NSLogFile()
{
}


void NSLogFile::clear()
{
	WriteMode t = currentWriteMode;
	currentWriteMode = OVERWRITE;

	writeDate();
	currentWriteMode = t;
}


/*-------------------------------------PUBLIC SET FUNCTIONS-------------------------------------------*/


/* setFileName
Description:
	Sets the log file name (as stored on disk)

Parameters:
	fileName : the name of the logfile to write to

Return:
	None
*/
void NSLogFile::setFileName(const nsstring & fileName)
{

	filename = fileName;
}


/* setDirectory
Description:
	Sets the log file directory

Parameters:
	directory : the directory to change dir to (the folder the log file is located in)
Return:
	None
*/
void NSLogFile::setDirectory(const nsstring & directory)
{

	dir = directory;
}


/* setWriteMode
Description:
	Sets the current write mode to the new one specified
	The write mode determines if the log file is overwritten on every new text entry or if it is simply 
	appended.
	The WriteMode enum type contains APPENDED and OVERWRITE modes which are self explanitory

Parameters:
	mode : the new write mode for the currentWriteMode to be set to

Return:
	None
*/
void NSLogFile::setWriteMode(WriteMode mode)
{
	currentWriteMode = mode;
}


/* setTimeStampEnabled
Description:
	Sets whether or not each text entree to be written to the log file will be timestamped or not.
	The timestamp appears before the text and separated by whitespace

Parameters:
	enabled : the bool value to say if timeStampEnabled is true or false

Return:
	None
*/
void NSLogFile::setTimeStampEnabled(bool enabled)
{

	timeStampEnabled = enabled;
}


/*-------------------------------------PUBLIC GET FUNCTIONS-------------------------------------------*/


/* getFileName
Description:
	Returns the log file name

Parameters:
	None

Return:
	const QString & : unmodifiable reference to the filename
*/
const nsstring & NSLogFile::getFileName() const
{

	return filename;
}


/* getDirectory
Description:
	Returns the log file directory

Parameters:
	None

Return:
	const QString & : unmodifiable reference to the directory
*/
const nsstring & NSLogFile::getDirectory() const
{
	return dir;
}


/* getWriteMode
Description:
	Returns the current write mode 

Parameters:
	NONE

Return:
	NSLogFile::WriteMode is the write mode type defined by the enum in nslogfile
*/
NSLogFile::WriteMode NSLogFile::getWriteMode() const
{

	return currentWriteMode;
}


/* isTimeStampEnabled
Description:
	Returns whether the current NSLogFile object has time stamping enabled

Parameters:
	NONE

Return:
	bool
*/
bool NSLogFile::isTimeStampEnabled() const
{

	return timeStampEnabled;
}


/*----------------------------------------- PUBLIC INTERFACE ------------------------------------------*/


/* write
Description:
	Writes text to the NSLogFile given in the dir directory with name of filename
	If mode is APPEND - the file is appended. If mode is OVERWRITE then erases contents and writes text 
	in place.
	If timeStampEnabled is true then a timestamp is written preceding the text entree.

Parameters:
	text : The text that is to be written to the NSLogFile

Return:
	bool : whether or not this function succeeded in writing to the file
*/
bool NSLogFile::write(const nsstring & text)
{
	nsstring fullFileName;

	// If there is a directory stored then use it for the log file, otherwise use the current directory
	if ( dir != "" )
		fullFileName = dir + "/" + filename;
	else
		fullFileName = filename;

	nsfstream outFile;
	
	switch (currentWriteMode)
	{
	case (APPEND):
		outFile.open(fullFileName,nsfstream::app);
		break;
	case (OVERWRITE):
		outFile.open(fullFileName,nsfstream::out);
		break;
	}
	
	if (!outFile.is_open())
		return false;

	auto now = std::chrono::system_clock::now();
	auto tt = std::chrono::system_clock::to_time_t(now);
	//if (timeStampEnabled)
		//outFile << std::put_time(std::localtime(&tt),"%T") << "    ";
	outFile << text << "\n";

	outFile.close();
	return true;
}


bool NSLogFile::write(const nsstringstream & ss)
{
	return write(ss.str());
}

bool NSLogFile::write(float num)
{
	nsstringstream ss;
	ss << num;
	return write(ss);
}

bool NSLogFile::write(int32 num)
{
	nsstringstream ss;
	ss << num;
	return write(ss);
}


/* writeDate
Description:
	This function simply writes the date in an obvious format to the file.
	Note - no matter what the mode the NSLogFile is in this function will always write the date with no 
	timestamp and in append mode

Parameters:
	None

Return:
	bool : whether or not this function succeeded in writing to the file
*/
bool NSLogFile::writeDate()
{
	nsstring fullFileName;

	// If there is a directory stored then use it for the log file, otherwise use the current directory
	if ( dir != "" )
	{
		fullFileName = dir + "/" + filename;
	}
	else
		fullFileName = filename;

	nsfstream outFile;
	switch (currentWriteMode)
	{
	case (APPEND):
		outFile.open(fullFileName,nsfstream::app);
		break;
	case (OVERWRITE):
		outFile.open(fullFileName,nsfstream::out);
		break;
	}

	if (!outFile.is_open())
		return false;

	auto now = std::chrono::system_clock::now();
	auto tt = std::chrono::system_clock::to_time_t(now);
	//outFile << std::put_time(std::localtime(&tt), "%c") << std::endl;

	outFile.close();
	return true;
}

/* static write
Description:
	Writes text to the logfile given with name of fname
	If mode is APPEND - the file is appended. If mode is OVERWRITE then erases contents and writes text
	in place.

Parameters:
	text : The text that is to be written to the logfile
	fname : the full path of the logfile to be written to - if just a name then will append/overwrite
	(or create) a log file with that name in the current working directory
	mode : the write mode - either append or overwrite

Return:
	bool : whether or not this function succeeded in writing to the file
*/
bool NSLogFile::writeTo(const nsstring & text, const nsstring &fname, WriteMode mode)
{
	if (fname.empty())
		return false;

	nsfstream outFile(fname);
	switch (mode)
	{
	case (APPEND):
		outFile.open(fname,nsfstream::app);
		break;
	case (OVERWRITE):
		outFile.open(fname,nsfstream::out);
		break;
	}

	if (!outFile.is_open())
		return false;

	auto now = std::chrono::system_clock::now();
	auto tt = std::chrono::system_clock::to_time_t(now);
	//outFile << std::put_time(std::localtime(&tt), "%c") << "    ";
	outFile.close();
	return true;
}

/* static writeDate
Description:
	This function simply writes the date in an obvious format to the file.

Parameters:
	fname : name of logfile to write the date to
	mode : write mode of the file - by default overwrites - can set to append

Return:
	bool : whether or not this function succeeded in writing to the file
*/
bool NSLogFile::writeDate(const nsstring & fname, WriteMode mode)
{
	nsfstream outFile(fname);
	switch (mode)
	{
	case (APPEND):
		outFile.open(fname,nsfstream::app);
		break;
	case (OVERWRITE):
		outFile.open(fname,nsfstream::out);
		break;
	}

	if (!outFile.is_open())
		return false;

	auto now = std::chrono::system_clock::now();
	auto tt = std::chrono::system_clock::to_time_t(now);
	//outFile << std::put_time(std::localtime(&tt), "%c") << std::endl;

	outFile.close();
	return true;
}

/* static writeToCom
Description:
	This function writes the text to the complete debug log file more of a helper function 
	than anything
	basically does a static write on the file "debugcom"

Parameters:
	text : the text to be written to the log file

Return:
	bool : whether or not this function succeeded in writing to the file
*/
bool NSLogFile::writeToCom(const nsstring & text, WriteMode mode )
{
	return writeTo(text, "Logs/debugcom.log",mode);
}

/*----------------------*
 | class NSLogFile ENDS   |
 *----------------------*/
