/*!
\file nsaudio_system.h

\brief Header file for nsaudio_system class

This file contains all of the neccessary declartations for the nsaudio_system class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/

#ifndef NSAUDIO_SYSTEM_H
#define NSAUDIO_SYSTEM_H

#include <nssystem.h>

struct ALCdevice_struct;
struct ALCcontext_struct;

void al_error_check(const nsstring & func);

class nsaudio_system : public nssystem
{
  public:
	
	nsaudio_system();

	~nsaudio_system();

	void init();

	void release();

	void update();

	int32 update_priority();

  private:
	
	ALCdevice_struct * m_device;
	ALCcontext_struct * m_ctxt;
};

#endif
