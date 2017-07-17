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

#include <system/nssystem.h>

class nsentity;

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

	uivec3 listener;
	float master_gain;
	float units_per_meter;
	
  private:

	bool handle_audio_play_event(audio_play_event * evnt);

	bool handle_audio_start_streaming_event(audio_start_streaming_event * evnt);
	
	bool handle_audio_stop_streaming_event(audio_stop_streaming_event * evnt);	
};

#endif
