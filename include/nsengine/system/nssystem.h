/*!
\file nssystem.h

\brief Header file for nssystem class

This file contains all of the neccessary declartations for the nssystem class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/

#ifndef NSSYSTEM_H
#define NSSYSTEM_H

#include <nsevent_handler.h>
#include <nsunordered_map.h>

class nstimer;
class nsscene;

class nssystem : public nsevent_handler
{
public:

	friend class nssys_factory;
	
	nssystem();

	virtual ~nssystem();

	virtual void init()=0;

	virtual void release() {}

	virtual void update() = 0;

	virtual int32 update_priority() = 0;

	uint32 type();

  protected:
	
	void add_trigger_hash(uint32 key, const nsstring & trigname);
	void remove_trigger_hash(uint32 key);
	uint32 trigger_hash(uint32 key);
	
  private:
	ui_ui_umap m_hashed_input_triggers;
	uint32 m_hashed_type;
};

#endif
