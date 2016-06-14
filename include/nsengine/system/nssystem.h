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
	
	nssystem(uint32 hashed_type);

	virtual ~nssystem();

	virtual void init()=0;

	virtual void release() {}

	virtual void update() = 0;

	nsscene * active_scene();

	virtual void set_active_scene(nsscene * active_scene);

	virtual int32 update_priority() = 0;

	bool scene_error_check();

	uint32 type();

  protected:

	bool m_scene_null_error;

	nsscene * m_active_scene;

  private:

	uint32 m_hashed_type;
};

#endif
