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
class nstform_ent_chunk;

class nssystem : public nsevent_handler
{
public:	
	nssystem(uint32 hashed_type);

	virtual ~nssystem();

	virtual void init() = 0;

	virtual void release() = 0;

	virtual void update() = 0;

	virtual void set_active_chunk(nstform_ent_chunk * active_chunk);

	virtual int32 update_priority() = 0;

	nstform_ent_chunk * active_chunk();

	bool chunk_error_check();

	uint32 type();

  protected:

	bool m_chunk_null_error;

	nstform_ent_chunk * m_active_chunk;

  private:

	uint32 m_hashed_type;
};

#endif
