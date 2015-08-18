/*!
\file nssystem.h

\brief Header file for NSSystem class

This file contains all of the neccessary declartations for the NSSystem class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/

#ifndef NSSYSTEM_H
#define NSSYSTEM_H

#include <nsglobal.h>
#include <nsevent_handler.h>

class NSTimer;
class nsscene;
class NSEvent;

class NSSystem : public NSEventHandler
{
public:

	friend class NSSysFactory;
	
	NSSystem();

	virtual ~NSSystem();

	virtual void draw();

	virtual void init()=0;

	virtual void update() = 0;

	virtual int32 draw_priority();

	virtual int32 update_priority() = 0;

	uint32 type();

  protected:
	
	void add_trigger_hash(uint32 key, const nsstring & trigname);
	void remove_trigger_hash(uint32 key);
	uint32 trigger_hash(uint32 key);
	
  private:
	uint32uintu_map m_hashed_input_triggers;
	uint32 m_hashed_type;
};

#endif
