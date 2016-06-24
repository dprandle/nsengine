/*! 
	\file nswindow.h
	
	\brief Header file for NSDebug class

	This file contains all of the neccessary declarations for the nswindow class.

	\author Daniel Randle
	\date November 7 2013
	\copywrite Earth Banana Games 2013
*/

#ifndef NSWINDOW_H
#define NSWINDOW_H

#include <nsmath.h>

struct GLFWwindow;

struct window_params
{
	bool fullscreen;
};

enum window_state
{
	minimized,
	maximized,
	full_screen,
	restored
};

class nswindow
{
	void show();

	void hide();

	bool is_open();

	bool visible();

	void set_state(window_state ws);

	window_state get_state();
	
	const fvec2 & cursor_pos();

	void poll_input();

	const fvec2 & size(bool pixel = true);

	void resize(const ivec2 & sz, bool pixel = true);

	void swap_buffers();

	void set_position(const ivec2 & pos);

	const ivec2 & position();

  private: // non-copyable or deleteable by anything except engine

	friend class nsengine;

	fvec2 m_size;
	fvec2 m_position;
	fvec2 m_norm_cpos;
	bool m_visible;
	window_state m_state;
	
	GLFWwindow * m_window;

	nswindow();
	~nswindow();
	nswindow(const nswindow & copy);
	nswindow & operator=(const nswindow & copy);

};


#endif
