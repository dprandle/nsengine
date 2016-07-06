/*! 
	\file nsrender_system.h
	
	\brief Header file for nsrender_system class

	This file contains all of the neccessary declarations for the nsrender_system class.

	\author Daniel Randle
	\date November 23 2013
	\copywrite Earth Banana Games 2013
*/

#ifndef NSRENDER_SYSTEM_H
#define NSRENDER_SYSTEM_H

// Input Events
#define VIEWPORT_CHANGE "mouse_pressed_in_viewport"

#include <nssystem.h>

class nstform_system : public nssystem
{
  public:
	
	nstform_system();
	~nstform_system();

	void init();

	void toggle_debug_draw();

	void update();

	virtual int32 update_priority();
	
  private:

	bool _handle_window_resize(window_resize_event * evt);
	bool _handle_viewport_change(nsaction_event * evt);
};

#endif
