/*! 
	\file nswindow.h
	
	\brief Header file for NSDebug class

	This file contains all of the neccessary declarations for the nswindow class.

	\author Daniel Randle
	\date November 7 2013
	\copywrite Earth Banana Games 2013
*/

#ifndef NSGL_WINDOW_H
#define NSGL_WINDOW_H

#include <nswindow.h>

#define EVENT_OUTPUT

struct GLFWwindow;
class nsgl_driver;
struct gl_ctxt;

class nsgl_window : public nswindow
{
	
	friend void glfw_key_press_callback(GLFWwindow * window_, int32 pKey, int32 scancode_, int32 action_, int32 mods_);
	friend void glfw_mouse_button_callback(GLFWwindow * window_, int32 pButton, int32 action_, int32 mods_);
	friend void glfw_cursor_pos_callback(GLFWwindow * window_, double x_pos, double y_pos);
	friend void glfw_scroll_callback(GLFWwindow * window_, double x_offset, double y_offset);
	friend void glfw_resize_window_callback(GLFWwindow* window, int32 width, int32 height);
	friend void glfw_focus_change_callback(GLFWwindow* window, int give_or_taken);
	friend void glfw_close_window_callback(GLFWwindow* window);
	friend void glfw_minimize_window_callback(GLFWwindow * window, int min_or_restore);
	friend void glfw_maximize_window_callback(GLFWwindow * window, int max_or_restore);
	friend void glfw_window_position_callback(GLFWwindow * window, int x_pos, int y_pos);

  public:
   
	nsgl_window(
		const ivec2 & window_size,
		const nsstring & win_title,
		const nsstring & win_icon_path="",
		uint8 creation_hint = win_hint_resizable | win_hint_decorated,
		window_state state_ = window_restored,
		const ivec2 & position = ivec2(),
		bool visible_ = true,
		bool focused_=true);

	~nsgl_window();

	void close();

	void render();

	void swap_buffers();
	
	void make_current();

	bool is_current();

	void set_visible(bool visible);

	void set_cursor_pos(const fvec2 & norm_cpos);

	void set_state(window_state ws);

	void resize(const ivec2 & sz);

	void set_position(const ivec2 & pos);

	void focus();

	void set_icon(nstex2d * icon);

	void set_title(const nsstring & title);

	gl_ctxt * vid_context();

  private:
	ivec2 m_pre_fs;
	nsgl_driver * m_driver;
	gl_ctxt * m_ctxt;
	GLFWwindow * m_window;	
};


#endif
