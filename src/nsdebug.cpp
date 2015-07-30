/*! 
	\file nsdebug.cpp
	
	\brief Definition file for NSDebug class

	This file contains all of the neccessary definitions for the NSDebug class.

	\author Daniel Randle
	\date November 7 2013
	\copywrite Earth Banana Games 2013
*/

#include <nsdebug.h>
#include <nslogfile.h>

NSDebug::NSDebug() : mConsoleOpen(false),
	mLogMessages(true), 
	mRenderMessages(true),
	mLevel(High),
	mMessageCallback(NULL),
	mMessageClassInstance(NULL),
	mLogFile(new NSLogFile())
{
	setLogFile(DEFAULT_DEBUG_LOG);
}

NSDebug::~NSDebug()
{
	delete mLogFile;
}
void NSDebug::clearLog()
{
	mLogFile->clear();
}

NSDebug::DebugLevel NSDebug::getDebugLevel() const
{
	return mLevel;
}

const nsstring & NSDebug::getLogFile() const
{
	return mLogFile->getFileName();
}

const MessageCallback NSDebug::getMessageCallback() const
{
	return mMessageCallback;
}

bool NSDebug::isConsoleOpen() const
{
	return mConsoleOpen;
}

void NSDebug::setLogDir(const nsstring & directory)
{
	mLogFile->setDirectory(directory);
}

bool NSDebug::isLogMessages() const
{
	return mLogMessages;
}

bool NSDebug::isRenderMessages() const
{
	return mRenderMessages;
}

void NSDebug::log(const nsstring & pMessage) const
{
	mLogFile->write(pMessage);
}

void NSDebug::print(const nsstring & pMessage) const
{
	if (mMessageCallback)
		mMessageCallback(pMessage,mMessageClassInstance);
	//if (mRenderMessages)
		//; write later
	if (mLogMessages)
		log(pMessage);
}

void NSDebug::setConsoleOpen(bool pConsoleOpen)
{
	mConsoleOpen = pConsoleOpen;
}

void NSDebug::setDebugLevel(DebugLevel pLevel)
{
	mLevel = pLevel;
}

void NSDebug::setLogFile(const nsstring & pFileName)
{
	mLogFile->setFileName(pFileName);
}

void NSDebug::setLogMessages(bool pLogMessages)
{
	mLogMessages = pLogMessages;
}

void NSDebug::setMessageCallback(MessageCallback pMessageCallback, ClassInstancePointer pMessageClassInstance)
{
	mMessageCallback = pMessageCallback;
	mMessageClassInstance = pMessageClassInstance;
}

void NSDebug::setRenderMessages(bool pRenderMessages)
{
	mRenderMessages = pRenderMessages;
}
