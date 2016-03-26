/*!
\file nsui_system.h

\brief Header file for nsui_system class

This file contains all of the neccessary declarations for the nsui_system class.

\author Daniel Randle
\date November 23 2013
\copywrite Earth Banana Games 2013
*/


#ifndef NSUI_SYSTEM_H
#define NSUI_SYSTEM_H

#define START_MAX_ENTS 1024

#include <nssystem.h>
#include <nsrender_system.h>

class nsui_shader;

class nsui_system : public nssystem
{
public:
	
	nsui_system();

	~nsui_system();

	virtual void init();

	virtual void release();

	virtual void update();

	int32 update_priority();

	void set_ui_shader(nsui_shader * shdr);

  private:
	void _sort_ents();
	std::vector<nsentity*> m_ordered_ents;
	std::vector<ui_draw_call> m_ui_draw_calls;
	nsui_shader * main_ui_shader;
};

#endif
