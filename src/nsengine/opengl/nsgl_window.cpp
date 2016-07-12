#include <nsgl_window.h>
#include <myGL/glew.h>
#include <GLFW/glfw3.h>
#include <nsengine.h>
#include <nsgl_driver.h>
#include <nsevent_dispatcher.h>
#include <nstexture.h>

void glfw_key_press_callback(GLFWwindow * window_, int32 key_, int32 scancode_, int32 action_, int32 mods_);
void glfw_mouse_button_callback(GLFWwindow * window_, int32 pButton, int32 action_, int32 mods_);
void glfw_cursor_pos_callback(GLFWwindow * window_, double x_pos, double y_pos);
void glfw_scroll_callback(GLFWwindow * window_, double x_offset, double y_offset);
void glfw_resize_window_callback(GLFWwindow* window, int32 width, int32 height);
void glfw_focus_change_callback(GLFWwindow* window, int give_or_taken);
void glfw_close_window_callback(GLFWwindow* window);
void glfw_minimize_window_callback(GLFWwindow * window, int min_or_restore);
void glfw_maximize_window_callback(GLFWwindow * window, int max_or_restore);
void glfw_window_position_callback(GLFWwindow * window, int x_pos, int y_pos);

void window_poll_input()
{
	glfwPollEvents();
}

nsgl_window::nsgl_window(
	const ivec2 & window_size,
	const nsstring & win_title,
	const nsstring & win_icon_path,
	uint8 creation_hint,
	window_state state_,
	const ivec2 & position_,
	bool visible_,
	bool focused_):
	nswindow(window_size, win_title, win_icon_path, creation_hint, state_, position_, visible_, focused_),
	m_window(nullptr),
	m_driver(nullptr),
	m_ctxt(nullptr),
	m_pre_fs(window_size)
{
	static uint8 window_count = 0;
	if (window_count == 0 && !glfwInit())
		return;
	
#ifdef GL_4_4
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
#elif defined(GL_4_1)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, user_resizable());
	glfwWindowHint(GLFW_VISIBLE, m_visible);
	glfwWindowHint(GLFW_DECORATED, decorated());
	glfwWindowHint(GLFW_AUTO_ICONIFY, true);
	glfwWindowHint(GLFW_FLOATING, always_on_top());
	glfwWindowHint(GLFW_MAXIMIZED, m_state == window_maximized);


	GLFWmonitor * mon = glfwGetPrimaryMonitor();
	if (m_state == window_full_screen)
		m_window = glfwCreateWindow(m_size.x, m_size.y, m_title.c_str(), mon, nullptr);
	else
		m_window = glfwCreateWindow(m_size.x, m_size.y, m_title.c_str(), nullptr, nullptr);

	int32 cnt = 0;
	const GLFWvidmode * array = glfwGetVideoModes(mon, &cnt);
	m_vid_modes.resize(cnt);

	for (int32 i = 0; i < cnt; ++i)
	{
		m_vid_modes[i].rgb_bit_depth.set(array[i].redBits,array[i].greenBits,array[i].blueBits);
		m_vid_modes[i].refresh_rate = array[i].refreshRate;
		m_vid_modes[i].resolution.set(array[i].width, array[i].height);
        if (m_vid_modes[i].resolution.x >= m_vid_modes[m_vid_mode_index].resolution.x
                && m_vid_modes[i].resolution.y >= m_vid_modes[m_vid_mode_index].resolution.y)
            m_vid_mode_index = i;
	}
	
    if (!m_window)
    {
        glfwTerminate();
        return;
    }
	++window_count;

	ivec2 fbsize;
	vec2 cpos;
	glfwGetFramebufferSize(m_window, &fbsize.w, &fbsize.h);
	glfwGetWindowSize(m_window, &m_size.w, &m_size.h);
	glfwGetWindowPos(m_window, &m_position.w, &m_position.h);

	glfwSetWindowUserPointer(m_window, (void*)this);
	glfwGetCursorPos(m_window, &cpos.x, &cpos.y);
	m_norm_cpos = fvec2(cpos) / fvec2(m_size);
	m_norm_cpos.y = 1.0f - m_norm_cpos.y;

	glfwSetKeyCallback(m_window, glfw_key_press_callback);
	glfwSetMouseButtonCallback(m_window, glfw_mouse_button_callback);
	glfwSetCursorPosCallback(m_window, glfw_cursor_pos_callback);
	glfwSetScrollCallback(m_window, glfw_scroll_callback);
    glfwSetFramebufferSizeCallback(m_window, glfw_resize_window_callback);
	glfwSetWindowFocusCallback(m_window, glfw_focus_change_callback);
	glfwSetWindowIconifyCallback(m_window, glfw_minimize_window_callback);
	glfwSetWindowCloseCallback(m_window, glfw_close_window_callback);
	glfwSetWindowPosCallback(m_window, glfw_window_position_callback);
	glfwMakeContextCurrent(m_window);
	glfwSwapInterval(1);

	m_driver = nse.video_driver<nsgl_driver>();
    m_driver->create_context();
    m_ctxt = m_driver->current_context();
	m_ctxt->setup_default_rendering();
    nse.event_dispatch()->push<window_resize_event>(m_ctxt->context_id, fbsize);
}

nsgl_window::~nsgl_window()
{}

void nsgl_window::close()
{
	GLFWwindow * curc = glfwGetCurrentContext();
	gl_ctxt * ctxt = m_driver->current_context();
    if (ctxt == m_ctxt)
        ctxt = nullptr;
    if (curc == m_window)
        curc = nullptr;
	
	make_current();
	m_driver->destroy_context(m_ctxt->context_id);
	glfwDestroyWindow(m_window);
	nswindow::close();

	glfwMakeContextCurrent(curc);
	if (ctxt != nullptr)
		m_driver->make_context_current(ctxt->context_id);
}

void nsgl_window::update()
{
	glfwSwapBuffers(m_window);	
}

void nsgl_window::make_current()
{
	glfwMakeContextCurrent(m_window);
	m_driver->make_context_current(m_ctxt->context_id);
}

bool nsgl_window::is_current()
{
	GLFWwindow * win = glfwGetCurrentContext();
	return (m_window == win && nse.video_driver()->current_context() == m_ctxt);
}


void nsgl_window::set_visible(bool visible)
{
	if (visible == m_visible || m_state == window_full_screen)
		return;
	if (visible)
		glfwShowWindow(m_window);
	else
		glfwHideWindow(m_window);
	m_visible = visible;
}

gl_ctxt * nsgl_window::vid_context()
{
	return m_ctxt;
}

void nsgl_window::set_cursor_pos(const fvec2 & sz)
{
	vec2 win_cpos = vec2(m_size) % vec2(sz.x,1.0f - sz.y);
	glfwSetCursorPos(m_window,win_cpos.x,win_cpos.y);
}

void nsgl_window::set_state(window_state ws)
{
	if (m_state == ws)
		return;
	
	if (ws != window_full_screen)
	{
		if (m_state == window_full_screen)
		{
			glfwRestoreWindow(m_window);
			glfwSetWindowMonitor(
				m_window, nullptr,
				m_position.y, m_position.y, // doesn't matter for fs
				m_pre_fs.x, m_pre_fs.y,
				GLFW_DONT_CARE);

			nse.event_dispatch()->push<nswindow_restored_event>(m_ctxt->context_id);
			m_state = ws;
		}

		if (ws == window_maximized)
			glfwMaximizeWindow(m_window);	
		else if (ws == window_minimized)
			glfwIconifyWindow(m_window);
		else
			glfwRestoreWindow(m_window);
	}
	else
	{
		if (m_state != window_restored)
		{
			glfwRestoreWindow(m_window);
		}
		GLFWmonitor * mon = glfwGetPrimaryMonitor();
		nse.event_dispatch()->push<nswindow_fullscreened_event>(m_ctxt->context_id, m_vid_modes[m_vid_mode_index].resolution);
		m_pre_fs = m_size;
		glfwSetWindowMonitor(
			m_window, mon,
			0, 0, // doesn't matter for fs
			m_vid_modes[m_vid_mode_index].resolution.x, m_vid_modes[m_vid_mode_index].resolution.y,
			m_vid_modes[m_vid_mode_index].refresh_rate);
		m_state = ws;
	}
}

void nsgl_window::resize(const ivec2 & sz)
{
	glfwSetWindowSize(m_window, sz.w, sz.h);
}

void nsgl_window::set_position(const ivec2 & sz)
{
	glfwSetWindowPos(m_window, sz.x, sz.y);
}

void nsgl_window::focus()
{
	glfwFocusWindow(m_window);
}

void nsgl_window::set_icon(nstex2d * icon)
{
	GLFWimage img;
	img.height = icon->size().h; img.width = icon->size().w;
	img.pixels = icon->data();
	glfwSetWindowIcon(m_window, 1, &img);
}

void nsgl_window::set_title(const nsstring & title)
{
	m_title = title;
	glfwSetWindowTitle(m_window, title.c_str());
}

void glfw_mouse_button_callback(GLFWwindow * window_, int32 pButton, int32 action_, int32 mods_)
{
	if (action_ == GLFW_REPEAT)
		return;

	nsgl_window * win = (nsgl_window*)glfwGetWindowUserPointer(window_);
	fvec2 norm_mpos = win->m_norm_cpos;
    bool pressed = action_ == 1;

    switch (pButton)
    {
    case (GLFW_MOUSE_BUTTON_LEFT) :
        nse.event_dispatch()->push<nsmouse_button_event>(nsinput_map::left_button, pressed, norm_mpos);
        break;
    case (GLFW_MOUSE_BUTTON_RIGHT) :
        nse.event_dispatch()->push<nsmouse_button_event>(nsinput_map::right_button, pressed, norm_mpos);
        break;
    case (GLFW_MOUSE_BUTTON_MIDDLE) :
        nse.event_dispatch()->push<nsmouse_button_event>(nsinput_map::middle_button, pressed, norm_mpos);
        break;
    case (GLFW_MOUSE_BUTTON_4) :
        nse.event_dispatch()->push<nsmouse_button_event>(nsinput_map::aux_button_1, pressed, norm_mpos);
        break;
    case (GLFW_MOUSE_BUTTON_5) :
        nse.event_dispatch()->push<nsmouse_button_event>(nsinput_map::aux_button_2, pressed, norm_mpos);
        break;
    case (GLFW_MOUSE_BUTTON_6) :
        nse.event_dispatch()->push<nsmouse_button_event>(nsinput_map::aux_button_3, pressed, norm_mpos);
        break;
    case (GLFW_MOUSE_BUTTON_7) :
        nse.event_dispatch()->push<nsmouse_button_event>(nsinput_map::aux_button_4, pressed, norm_mpos);
        break;
    default:
        break;
    }
}

void glfw_cursor_pos_callback(GLFWwindow * window_, double x_pos, double y_pos)
{
	nsgl_window * win = (nsgl_window*)glfwGetWindowUserPointer(window_);
	win->m_norm_cpos = fvec2(x_pos,y_pos) / fvec2(win->m_size);
	win->m_norm_cpos.y = 1 - win->m_norm_cpos.y;
    nse.event_dispatch()->push<nsmouse_move_event>(win->m_norm_cpos);
}

void glfw_scroll_callback(GLFWwindow * window_, double x_offset, double y_offset)
{
	nsgl_window * win = (nsgl_window*)glfwGetWindowUserPointer(window_);
    nse.event_dispatch()->push<nsmouse_scroll_event>(float(y_offset), win->m_norm_cpos);
}

void glfw_resize_window_callback(GLFWwindow* window, int32 width, int32 height)
{
	
	nsgl_window * win = (nsgl_window*)glfwGetWindowUserPointer(window);
	win->m_size = ivec2(width,height);
	int ret = glfwGetWindowAttrib(window, GLFW_MAXIMIZED);
	if (ret)
	{
		glfw_maximize_window_callback(window, 1);
	}
	else
	{
		if (win->m_state == window_maximized)
			glfw_maximize_window_callback(window, 0);
	}
    nse.event_dispatch()->push<window_resize_event>(0,win->m_size);
#ifdef EVENT_OUTPUT
    std::cout << "window " << win->m_ctxt->context_id << " resize callback" << std::endl;
#endif
}

void glfw_focus_change_callback(GLFWwindow* window, int give_or_taken)
{
	nsgl_window * win = (nsgl_window*)glfwGetWindowUserPointer(window);
	if (give_or_taken)
	{
		win->m_focused = true;
        win->make_current();
#ifdef EVENT_OUTPUT
    std::cout << "window " << win->m_ctxt->context_id << " gained focus callback" << std::endl;
#endif
		nse.event_dispatch()->push<nswindow_focused_event>(win->m_ctxt->context_id);
	}
	else
	{
		win->m_focused = false;
		nse.event_dispatch()->push<nswindow_unfocused_event>(win->m_ctxt->context_id);
	}
}

void glfw_close_window_callback(GLFWwindow* window)
{
	nsgl_window * win = (nsgl_window*)glfwGetWindowUserPointer(window);
	win->close();
	nse.event_dispatch()->push<nswindow_closed_event>(win->m_ctxt->context_id);
#ifdef EVENT_OUTPUT
    std::cout << "window " << win->m_ctxt->context_id << " close callback" << std::endl;
#endif
}

void glfw_minimize_window_callback(GLFWwindow * window, int min_or_restore)
{
	nsgl_window * win = (nsgl_window*)glfwGetWindowUserPointer(window);
	if (min_or_restore)
	{
		win->m_state = window_minimized;
		nse.event_dispatch()->push<nswindow_minimized_event>(win->m_ctxt->context_id);
#ifdef EVENT_OUTPUT
        std::cout << "minimize window " << win->m_ctxt->context_id << " callback" << std::endl;
#endif
	}
	else
	{
		win->m_state = window_restored;
		nse.event_dispatch()->push<nswindow_restored_event>(win->m_ctxt->context_id);
#ifdef EVENT_OUTPUT
        std::cout << "minimize window " << win->m_ctxt->context_id << " restore callback" << std::endl;
#endif
	}
}

void glfw_maximize_window_callback(GLFWwindow * window, int max_or_restore)
{
	nsgl_window * win = (nsgl_window*)glfwGetWindowUserPointer(window);
	if (max_or_restore)
	{
		win->m_state = window_maximized;
		nse.event_dispatch()->push<nswindow_maximized_event>(win->m_ctxt->context_id);
#ifdef EVENT_OUTPUT
    std::cout << "maximize window " << win->m_ctxt->context_id << " callback" << std::endl;
#endif
	}
	else
	{
		win->m_state = window_restored;
		nse.event_dispatch()->push<nswindow_restored_event>(win->m_ctxt->context_id);
#ifdef EVENT_OUTPUT
    std::cout << "maximize window " << win->m_ctxt->context_id << " restore callback" << std::endl;
#endif
	}
}

void glfw_window_position_callback(GLFWwindow * window, int x_pos, int y_pos)
{
	nsgl_window * win = (nsgl_window*)glfwGetWindowUserPointer(window);
	win->m_position = ivec2(x_pos, y_pos);
	nse.event_dispatch()->push<nswindow_position_event>(win->m_ctxt->context_id, win->m_position);
}

void glfw_key_press_callback(GLFWwindow * window_, int32 key_, int32 scancode_, int32 action_, int32 mods_)
{
	if (action_ == GLFW_REPEAT)
		return;
	
    bool pressed = action_ == 1;

    switch (key_)
    {
    case (GLFW_KEY_A) :
        nse.event_dispatch()->push<nskey_event>(nsinput_map::key_a, pressed);
        break;
    case (GLFW_KEY_B) :
        nse.event_dispatch()->push<nskey_event>(nsinput_map::key_b, pressed);
        break;
    case (GLFW_KEY_C) :
        nse.event_dispatch()->push<nskey_event>(nsinput_map::key_c, pressed);
        break;
    case (GLFW_KEY_D) :
        nse.event_dispatch()->push<nskey_event>(nsinput_map::key_d, pressed);
        break;
    case (GLFW_KEY_E) :
        nse.event_dispatch()->push<nskey_event>(nsinput_map::key_e, pressed);
        break;
    case (GLFW_KEY_F) :
        nse.event_dispatch()->push<nskey_event>(nsinput_map::key_f, pressed);
        break;
    case (GLFW_KEY_G) :
        nse.event_dispatch()->push<nskey_event>(nsinput_map::key_g, pressed);
        break;
    case (GLFW_KEY_H) :
        nse.event_dispatch()->push<nskey_event>(nsinput_map::key_h, pressed);
        break;
    case (GLFW_KEY_I) :
        nse.event_dispatch()->push<nskey_event>(nsinput_map::key_i, pressed);
        break;
    case (GLFW_KEY_J) :
        nse.event_dispatch()->push<nskey_event>(nsinput_map::key_j, pressed);
        break;
    case (GLFW_KEY_K) :
        nse.event_dispatch()->push<nskey_event>(nsinput_map::key_k, pressed);
        break;
    case (GLFW_KEY_L) :
        nse.event_dispatch()->push<nskey_event>(nsinput_map::key_l, pressed);
        break;
    case (GLFW_KEY_M) :
        nse.event_dispatch()->push<nskey_event>(nsinput_map::key_m, pressed);
        break;
    case (GLFW_KEY_N) :
        nse.event_dispatch()->push<nskey_event>(nsinput_map::key_n, pressed);
        break;
    case (GLFW_KEY_O) :
        nse.event_dispatch()->push<nskey_event>(nsinput_map::key_o, pressed);
        break;
    case (GLFW_KEY_P) :
        nse.event_dispatch()->push<nskey_event>(nsinput_map::key_p, pressed);
        break;
    case (GLFW_KEY_Q) :
        nse.event_dispatch()->push<nskey_event>(nsinput_map::key_q, pressed);
        break;
    case (GLFW_KEY_R) :
        nse.event_dispatch()->push<nskey_event>(nsinput_map::key_r, pressed);
        break;
    case (GLFW_KEY_S) :
        nse.event_dispatch()->push<nskey_event>(nsinput_map::key_s, pressed);
        break;
    case (GLFW_KEY_T) :
        nse.event_dispatch()->push<nskey_event>(nsinput_map::key_t, pressed);
        break;
    case (GLFW_KEY_U) :
        nse.event_dispatch()->push<nskey_event>(nsinput_map::key_u, pressed);
        break;
    case (GLFW_KEY_V) :
        nse.event_dispatch()->push<nskey_event>(nsinput_map::key_v, pressed);
        break;
    case (GLFW_KEY_W) :
        nse.event_dispatch()->push<nskey_event>(nsinput_map::key_w, pressed);
        break;
    case (GLFW_KEY_X) :
        nse.event_dispatch()->push<nskey_event>(nsinput_map::key_x, pressed);
        break;
    case (GLFW_KEY_Y) :
        nse.event_dispatch()->push<nskey_event>(nsinput_map::key_y, pressed);
        break;
    case (GLFW_KEY_Z) :
        nse.event_dispatch()->push<nskey_event>(nsinput_map::key_z, pressed);
        break;
    case (GLFW_KEY_0) :
        nse.event_dispatch()->push<nskey_event>(nsinput_map::key_0, pressed);
        break;
    case (GLFW_KEY_1) :
        nse.event_dispatch()->push<nskey_event>(nsinput_map::key_1, pressed);
        break;
    case (GLFW_KEY_2) :
        nse.event_dispatch()->push<nskey_event>(nsinput_map::key_2, pressed);
        break;
    case (GLFW_KEY_3) :
        nse.event_dispatch()->push<nskey_event>(nsinput_map::key_3, pressed);
        break;
    case (GLFW_KEY_4) :
        nse.event_dispatch()->push<nskey_event>(nsinput_map::key_4, pressed);
        break;
    case (GLFW_KEY_5) :
        nse.event_dispatch()->push<nskey_event>(nsinput_map::key_5, pressed);
        break;
    case (GLFW_KEY_6) :
        nse.event_dispatch()->push<nskey_event>(nsinput_map::key_6, pressed);
        break;
    case (GLFW_KEY_7) :
        nse.event_dispatch()->push<nskey_event>(nsinput_map::key_7, pressed);
        break;
    case (GLFW_KEY_8) :
        nse.event_dispatch()->push<nskey_event>(nsinput_map::key_8, pressed);
        break;
    case (GLFW_KEY_9) :
        nse.event_dispatch()->push<nskey_event>(nsinput_map::key_9, pressed);
        break;
    case (GLFW_KEY_GRAVE_ACCENT) :
        nse.event_dispatch()->push<nskey_event>(nsinput_map::key_grave_accent, pressed);
        break;
    case (GLFW_KEY_MINUS) :
        nse.event_dispatch()->push<nskey_event>(nsinput_map::key_minus, pressed);
        break;
    case (GLFW_KEY_EQUAL) :
        nse.event_dispatch()->push<nskey_event>(nsinput_map::key_equal, pressed);
        break;
    case (GLFW_KEY_BACKSPACE) :
        nse.event_dispatch()->push<nskey_event>(nsinput_map::key_backspace, pressed);
        break;
    case (GLFW_KEY_TAB) :
        nse.event_dispatch()->push<nskey_event>(nsinput_map::key_tab, pressed);
        break;
    case (GLFW_KEY_LEFT_BRACKET) :
        nse.event_dispatch()->push<nskey_event>(nsinput_map::key_lbracket, pressed);
        break;
    case (GLFW_KEY_RIGHT_BRACKET) :
        nse.event_dispatch()->push<nskey_event>(nsinput_map::key_rbracket, pressed);
        break;
    case (GLFW_KEY_BACKSLASH) :
        nse.event_dispatch()->push<nskey_event>(nsinput_map::key_backslash, pressed);
        break;
    case (GLFW_KEY_CAPS_LOCK) :
        nse.event_dispatch()->push<nskey_event>(nsinput_map::key_capslock, pressed);
        break;
    case (GLFW_KEY_SEMICOLON) :
        nse.event_dispatch()->push<nskey_event>(nsinput_map::key_semicolon, pressed);
        break;
    case (GLFW_KEY_APOSTROPHE) :
        nse.event_dispatch()->push<nskey_event>(nsinput_map::key_apostrophe, pressed);
        break;
    case (GLFW_KEY_ENTER) :
        nse.event_dispatch()->push<nskey_event>(nsinput_map::key_enter, pressed);
        break;
    case (GLFW_KEY_LEFT_SHIFT) :
        nse.event_dispatch()->push<nskey_event>(nsinput_map::key_lshift, pressed);
        break;
    case (GLFW_KEY_COMMA) :
        nse.event_dispatch()->push<nskey_event>(nsinput_map::key_comma, pressed);
        break;
    case (GLFW_KEY_PERIOD) :
        nse.event_dispatch()->push<nskey_event>(nsinput_map::key_period, pressed);
        break;
    case (GLFW_KEY_SLASH) :
        nse.event_dispatch()->push<nskey_event>(nsinput_map::key_forwardslash, pressed);
        break;
    case (GLFW_KEY_RIGHT_SHIFT) :
        nse.event_dispatch()->push<nskey_event>(nsinput_map::key_rshift, pressed);
        break;
    case (GLFW_KEY_LEFT_CONTROL) :
        nse.event_dispatch()->push<nskey_event>(nsinput_map::key_lctrl, pressed);
        break;
    case (GLFW_KEY_LEFT_SUPER) :
        nse.event_dispatch()->push<nskey_event>(nsinput_map::key_lsuper, pressed);
        break;
    case (GLFW_KEY_LEFT_ALT) :
        nse.event_dispatch()->push<nskey_event>(nsinput_map::key_lalt, pressed);
        break;
    case (GLFW_KEY_SPACE) :
        nse.event_dispatch()->push<nskey_event>(nsinput_map::key_space, pressed);
        break;
    case (GLFW_KEY_RIGHT_ALT) :
        nse.event_dispatch()->push<nskey_event>(nsinput_map::key_ralt, pressed);
        break;
    case (GLFW_KEY_RIGHT_SUPER) :
        nse.event_dispatch()->push<nskey_event>(nsinput_map::key_rsuper, pressed);
        break;
    case (GLFW_KEY_RIGHT_CONTROL) :
        nse.event_dispatch()->push<nskey_event>(nsinput_map::key_rctrl, pressed);
        break;
    case (GLFW_KEY_LEFT) :
        nse.event_dispatch()->push<nskey_event>(nsinput_map::key_left, pressed);
        break;
    case (GLFW_KEY_RIGHT) :
        nse.event_dispatch()->push<nskey_event>(nsinput_map::key_right, pressed);
        break;
    case (GLFW_KEY_UP) :
        nse.event_dispatch()->push<nskey_event>(nsinput_map::key_up, pressed);
        break;
    case (GLFW_KEY_DOWN) :
        nse.event_dispatch()->push<nskey_event>(nsinput_map::key_down, pressed);
        break;
    case (GLFW_KEY_ESCAPE) :
        nse.event_dispatch()->push<nskey_event>(nsinput_map::key_esc, pressed);
        break;
    case (GLFW_KEY_F1) :
        nse.event_dispatch()->push<nskey_event>(nsinput_map::key_f1, pressed);
        break;
    case (GLFW_KEY_F2) :
        nse.event_dispatch()->push<nskey_event>(nsinput_map::key_f2, pressed);
        break;
    case (GLFW_KEY_F3) :
        nse.event_dispatch()->push<nskey_event>(nsinput_map::key_f3, pressed);
        break;
    case (GLFW_KEY_F4) :
        nse.event_dispatch()->push<nskey_event>(nsinput_map::key_f4, pressed);
        break;
    case (GLFW_KEY_F5) :
        nse.event_dispatch()->push<nskey_event>(nsinput_map::key_f5, pressed);
        break;
    case (GLFW_KEY_F6) :
        nse.event_dispatch()->push<nskey_event>(nsinput_map::key_f6, pressed);
        break;
    case (GLFW_KEY_F7) :
        nse.event_dispatch()->push<nskey_event>(nsinput_map::key_f7, pressed);
        break;
    case (GLFW_KEY_F8) :
        nse.event_dispatch()->push<nskey_event>(nsinput_map::key_f8, pressed);
        break;
    case (GLFW_KEY_F9) :
        nse.event_dispatch()->push<nskey_event>(nsinput_map::key_f9, pressed);
        break;
    case (GLFW_KEY_F10) :
        nse.event_dispatch()->push<nskey_event>(nsinput_map::key_f10, pressed);
        break;
    case (GLFW_KEY_F11) :
        nse.event_dispatch()->push<nskey_event>(nsinput_map::key_f11, pressed);
        break;
    case (GLFW_KEY_F12) :
        nse.event_dispatch()->push<nskey_event>(nsinput_map::key_f12, pressed);
        break;
    case (GLFW_KEY_F13) :
        nse.event_dispatch()->push<nskey_event>(nsinput_map::key_f13, pressed);
        break;
    case (GLFW_KEY_F14) :
        nse.event_dispatch()->push<nskey_event>(nsinput_map::key_f14, pressed);
        break;
    case (GLFW_KEY_PRINT_SCREEN) :
        nse.event_dispatch()->push<nskey_event>(nsinput_map::key_print_screen, pressed);
        break;
    case (GLFW_KEY_SCROLL_LOCK) :
        nse.event_dispatch()->push<nskey_event>(nsinput_map::key_scroll_lock, pressed);
        break;
    case (GLFW_KEY_PAUSE) :
        nse.event_dispatch()->push<nskey_event>(nsinput_map::key_pause, pressed);
        break;
    case (GLFW_KEY_INSERT) :
        nse.event_dispatch()->push<nskey_event>(nsinput_map::key_insert, pressed);
        break;
    case (GLFW_KEY_DELETE) :
        nse.event_dispatch()->push<nskey_event>(nsinput_map::key_delete, pressed);
        break;
    case (GLFW_KEY_PAGE_UP) :
        nse.event_dispatch()->push<nskey_event>(nsinput_map::key_page_up, pressed);
        break;
    case (GLFW_KEY_PAGE_DOWN) :
        nse.event_dispatch()->push<nskey_event>(nsinput_map::key_page_down, pressed);
        break;
    case (GLFW_KEY_HOME) :
        nse.event_dispatch()->push<nskey_event>(nsinput_map::key_home, pressed);
        break;
    case (GLFW_KEY_END) :
        nse.event_dispatch()->push<nskey_event>(nsinput_map::key_end, pressed);
        break;
    case (GLFW_KEY_NUM_LOCK) :
        nse.event_dispatch()->push<nskey_event>(nsinput_map::key_numlock, pressed);
        break;
    case (GLFW_KEY_KP_DIVIDE) :
        nse.event_dispatch()->push<nskey_event>(nsinput_map::key_keypad_divide, pressed);
        break;
    case (GLFW_KEY_KP_MULTIPLY) :
        nse.event_dispatch()->push<nskey_event>(nsinput_map::key_keypad_multiply, pressed);
        break;
    case (GLFW_KEY_KP_SUBTRACT) :
        nse.event_dispatch()->push<nskey_event>(nsinput_map::key_keypad_subtract, pressed);
        break;
    case (GLFW_KEY_KP_ADD) :
        nse.event_dispatch()->push<nskey_event>(nsinput_map::key_keypad_add, pressed);
        break;
    case (GLFW_KEY_KP_DECIMAL) :
        nse.event_dispatch()->push<nskey_event>(nsinput_map::key_keypad_decimal, pressed);
        break;
    case (GLFW_KEY_KP_ENTER) :
        nse.event_dispatch()->push<nskey_event>(nsinput_map::key_keypad_enter, pressed);
        break;
    case (GLFW_KEY_KP_EQUAL) :
        nse.event_dispatch()->push<nskey_event>(nsinput_map::key_keypad_equal, pressed);
        break;
    case (GLFW_KEY_KP_0) :
        nse.event_dispatch()->push<nskey_event>(nsinput_map::key_keypad_0, pressed);
        break;
    case (GLFW_KEY_KP_1) :
        nse.event_dispatch()->push<nskey_event>(nsinput_map::key_keypad_1, pressed);
        break;
    case (GLFW_KEY_KP_2) :
        nse.event_dispatch()->push<nskey_event>(nsinput_map::key_keypad_2, pressed);
        break;
    case (GLFW_KEY_KP_3) :
        nse.event_dispatch()->push<nskey_event>(nsinput_map::key_keypad_3, pressed);
        break;
    case (GLFW_KEY_KP_4) :
        nse.event_dispatch()->push<nskey_event>(nsinput_map::key_keypad_4, pressed);
        break;
    case (GLFW_KEY_KP_5) :
        nse.event_dispatch()->push<nskey_event>(nsinput_map::key_keypad_5, pressed);
        break;
    case (GLFW_KEY_KP_6) :
        nse.event_dispatch()->push<nskey_event>(nsinput_map::key_keypad_6, pressed);
        break;
    case (GLFW_KEY_KP_7) :
        nse.event_dispatch()->push<nskey_event>(nsinput_map::key_keypad_7, pressed);
        break;
    case (GLFW_KEY_KP_8) :
        nse.event_dispatch()->push<nskey_event>(nsinput_map::key_keypad_8, pressed);
        break;
    case (GLFW_KEY_KP_9) :
        nse.event_dispatch()->push<nskey_event>(nsinput_map::key_keypad_9, pressed);
        break;
    default:
        break;
    }
}
