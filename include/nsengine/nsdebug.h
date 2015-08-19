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

class NSLogFile;

typedef void (*message_callback)(const nsstring &, void *);

/*!
NSDebug handles all debug operations

\bug No known bugs.
*/
class nsdebug
{
public:
	enum debug_level {
		deb_low,
		deb_medium,
		deb_high };

	nsdebug();

	~nsdebug();
	
	void clear_log();

	debug_level get_debug_level() const;

	const nsstring & get_log_file() const;

	const message_callback get_message_callback() const;

	bool console_open() const;

	bool log_messages() const;
	
	bool render_messages() const;

	void log(const nsstring & pMessage) const;

	void print(const nsstring & pMessage) const;

	void set_console_open(bool pConsoleOpen);

	void set_debug_level(debug_level pLevel);

	void set_log_file(const nsstring & pFileName);

	void set_log_dir(const nsstring & directory);

	void set_log_messages(bool pLogMessages);

	void set_message_callback(message_callback pMessageCallback, void * pMessageClassInstance=NULL);

	void set_render_messages(bool pRenderMessages);

private:
	bool m_console_open;
	bool m_logging_messages;
	bool m_render_messages;
	debug_level m_deb_level;
	message_callback m_msg_cb;
	void * m_msg_inst;
	NSLogFile* m_log_file;
};

#endif
