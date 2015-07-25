/*! 
	\file nsdebug.h
	
	\brief Header file for NSDebug class

	This file contains all of the neccessary declarations for the NSDebug class.

	\author Daniel Randle
	\date November 7 2013
	\copywrite Earth Banana Games 2013
*/

#ifndef NSDEBUG_H
#define NSDEBUG_H

#include <nsglobal.h>

class SomeClass;
class NSLogFile;

typedef void* ClassInstancePointer;
typedef void (*MessageCallback)(const nsstring &, ClassInstancePointer);

/*!
NSDebug handles all debug operations

\bug No known bugs.
*/
class NSDebug
{
public:
	enum DebugLevel {
		Low,
		Medium,
		High };

	NSDebug();
	~NSDebug();
	
	void clearLog();

	DebugLevel getDebugLevel() const;
	const nsstring & getLogFile() const;
	const MessageCallback getMessageCallback() const;

	bool isConsoleOpen() const;
	bool isLogMessages() const;
	bool isRenderMessages() const;

	void log(const nsstring & pMessage) const;

	void print(const nsstring & pMessage) const;

	void setConsoleOpen(bool pConsoleOpen);
	void setDebugLevel(DebugLevel pLevel);
	void setLogFile(const nsstring & pFileName);
	void setLogMessages(bool pLogMessages);
	void setMessageCallback(MessageCallback pMessageCallback, ClassInstancePointer pMessageClassInstance=NULL);
	void setRenderMessages(bool pRenderMessages);

private:
	bool mConsoleOpen;
	bool mLogMessages;
	bool mRenderMessages;
	DebugLevel mLevel;
	MessageCallback mMessageCallback;
	ClassInstancePointer mMessageClassInstance;

	NSLogFile* mLogFile;
};

#endif