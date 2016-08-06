/*! 
	\file nsdebug.cpp
	
	\brief Definition file for NSDebug class

	This file contains all of the neccessary definitions for the NSDebug class.

	\author Daniel Randle
	\date November 7 2013
	\copywrite Earth Banana Games 2013
*/

#include <nsdebug.h>
#include <nslog_file.h>
#include <nsplatform.h>

nsdebug::nsdebug() : m_console_open(false),
	m_logging_messages(true), 
	m_render_messages(true),
	m_deb_level(deb_high),
	m_msg_cb(NULL),
	m_msg_inst(NULL),
	m_log_file(new nslog_file())
{
	set_log_file(DEFAULT_DEBUG_LOG);
}

nsdebug::~nsdebug()
{
	delete m_log_file;
}

void nsdebug::clear_log()
{
	m_log_file->clear();
}

nsdebug::debug_level nsdebug::get_debug_level() const
{
	return m_deb_level;
}

const nsstring & nsdebug::get_log_file() const
{
	return m_log_file->file_name();
}

const message_callback & nsdebug::get_message_callback() const
{
	return m_msg_cb;
}

bool nsdebug::console_open() const
{
	return m_console_open;
}

void nsdebug::set_log_dir(const nsstring & directory)
{
	m_log_file->set_dir(directory);
}

bool nsdebug::log_messages() const
{
	return m_logging_messages;
}

bool nsdebug::render_messages() const
{
	return m_render_messages;
}

void nsdebug::log(const nsstring & pMessage) const
{
	m_log_file->write(pMessage);
}

void nsdebug::print(const nsstring & pMessage) const
{
	if (m_msg_cb)
		m_msg_cb(pMessage,m_msg_inst);
	//if (mRenderMessages)
	//; write later
	if (m_logging_messages)
		log(pMessage);
}

void nsdebug::set_console_open(bool pConsoleOpen)
{
	m_console_open = pConsoleOpen;
}

void nsdebug::set_debug_level(debug_level pLevel)
{
	m_deb_level = pLevel;
}

void nsdebug::set_log_file(const nsstring & pFileName)
{
	m_log_file->set_file_name(pFileName);
}

void nsdebug::set_log_messages(bool pLogMessages)
{
	m_logging_messages = pLogMessages;
}

void nsdebug::set_message_callback(message_callback pMessageCallback, void * pMessageClassInstance)
{
	m_msg_cb = pMessageCallback;
	m_msg_inst = pMessageClassInstance;
}

void nsdebug::set_render_messages(bool pRenderMessages)
{
	m_render_messages = pRenderMessages;
}

#ifdef WIN32
#include <Windows.h>
#include <DbgHelp.h>
int nsdebug_dump::save(const nsstring & fname, void * param, info_level ilevel)
{

    int type = MiniDumpNormal;
    switch (ilevel)
    {
	  case (info_level_small):
		  type |= MiniDumpWithIndirectlyReferencedMemory |
			  MiniDumpScanMemory;
		  break;
	  case (info_level_medium):
		  type |= MiniDumpWithDataSegs |
			  MiniDumpWithPrivateReadWriteMemory |
			  MiniDumpWithHandleData |
			  MiniDumpWithFullMemoryInfo |
			  MiniDumpWithThreadInfo |
			  MiniDumpWithUnloadedModules;
		  break;
	  case (info_level_large):
		  type |= MiniDumpWithDataSegs |
			  MiniDumpWithPrivateReadWriteMemory |
			  MiniDumpWithHandleData |
			  MiniDumpWithFullMemory |
			  MiniDumpWithFullMemoryInfo |
			  MiniDumpWithThreadInfo |
			  MiniDumpWithUnloadedModules |
			  MiniDumpWithProcessThreadData;
		  break;
    }

    int success;
    MINIDUMP_EXCEPTION_INFORMATION ExpParam;

    platform::create_dir(fname);
    HANDLE hDumpFile = CreateFile(
		fname.c_str(),
		GENERIC_READ|GENERIC_WRITE,
		FILE_SHARE_WRITE|FILE_SHARE_READ,
		0,
		CREATE_ALWAYS,
		0,
		0);

    ExpParam.ThreadId = GetCurrentThreadId();
    ExpParam.ExceptionPointers = (EXCEPTION_POINTERS*)param;
    ExpParam.ClientPointers = TRUE;

    success = MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(),
								hDumpFile, (MINIDUMP_TYPE)type, &ExpParam, NULL, NULL);

    return success;
}
#else
int nsdebug_dump::save(const nsstring & fname, void * param, info_level ilevel)
{
    return 0;
	
}
#endif
