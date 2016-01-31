#ifndef NSGLFW_H
#define NSGLFW_H

#include <nsmath.h>

struct GLFWwindow;

bool glfw_setup(const ivec2 & screendim, bool fullscreen, const nsstring & title);

void glfw_shutdown();

bool glfw_window_open();

void glfw_update();

void glfw_keypress_callback(GLFWwindow * pWindow, int32 pKey, int32 pScancode, int32 pAction, int32 pMods);

void glfw_mousebutton_callback(GLFWwindow * pWindow, int32 pButton, int32 pAction, int32 pMods);

void glfw_cursorpos_callback(GLFWwindow * pWindow, double pPosX, double pPosY);

void glfw_scroll_callback(GLFWwindow * pWindow, double pXOffset, double pYOffset);

void glfw_resizewindow_callback(GLFWwindow* window, int32 width, int32 height);


#include <myGL/glew.h>
#include <GLFW/glfw3.h>
#include <nsglfw.h>
#include <iostream>
#include <nsevent.h>
#include <nsevent_dispatcher.h>
#include <nsengine.h>
#include <nsrender_system.h>
#include <nsinput_system.h>

GLFWwindow * win;

fvec2 platform_normalized_mpos()
{

    double xPos = 0.0, yPos = 0.0; int32 frameBufX = 0, frameBufY = 0, winX = 0, winY = 0;
    glfwGetCursorPos(win, &xPos, &yPos);
    glfwGetFramebufferSize(win, &frameBufX, &frameBufY);
    glfwGetWindowSize(win, &winX, &winY);

    yPos = winY - yPos; // Switch to opengl coords

    // normalize coords
    yPos /= double(frameBufY);
    xPos /= double(frameBufX);

    // so compilers wont complain turn in to floats explicitly
    float normXPos = float(xPos);
    float normYPos = float(yPos);

	return fvec2(normXPos,normYPos);
}

bool glfw_setup(const ivec2 & screendim, bool fullscreen, const nsstring & title)
{
    if (!glfwInit())
        return false;

	GLFWmonitor * mon = NULL;
	if (fullscreen)
		mon = glfwGetPrimaryMonitor();		
	win = glfwCreateWindow(screendim.x, screendim.y, title.c_str(), mon, NULL);
		
    if (!win)
    {
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(win);
    glfwSetKeyCallback(win, glfw_keypress_callback);
	glfwSetMouseButtonCallback(win, glfw_mousebutton_callback);
	glfwSetCursorPosCallback(win, glfw_cursorpos_callback);
	glfwSetScrollCallback(win, glfw_scroll_callback);
	glfwSetFramebufferSizeCallback(win, glfw_resizewindow_callback); 
    return true;
}

void glfw_shutdown()
{
	glfwTerminate();
}

bool glfw_window_open()
{
	return !glfwWindowShouldClose(win);
}

void glfw_update()
{
	glfwSwapBuffers(win);
	glfwPollEvents();
}

void glfw_keypress_callback(GLFWwindow * pWindow, int32 pKey, int32 pScancode, int32 pAction, int32 pMods)
{
	if (pAction == GLFW_REPEAT)
		return;
	
    bool pressed = pAction == 1;
    switch (pKey)
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

void glfw_mousebutton_callback(GLFWwindow * pWindow, int32 pButton, int32 pAction, int32 pMods)
{
	if (pAction == GLFW_REPEAT)
		return;

	fvec2 norm_mpos = platform_normalized_mpos();
    bool pressed = pAction == 1;

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

void glfw_cursorpos_callback(GLFWwindow * pWindow, double pPosX, double pPosY)
{
    int32 frameBufX = 0, frameBufY = 0, winX = 0, winY = 0;
    glfwGetWindowSize(pWindow, &winX, &winY);
    pPosY = winY - pPosY; // Switch to opengl coords
    // normalize coords
    pPosY /= double(winY);
    pPosX /= double(winX);
    nse.event_dispatch()->push<nsmouse_move_event>(fvec2(float(pPosX),float(pPosY)));
}

void glfw_scroll_callback(GLFWwindow * pWindow, double pXOffset, double pYOffset)
{
	fvec2 norm_mpos = platform_normalized_mpos();
    nse.event_dispatch()->push<nsmouse_scroll_event>(float(pYOffset), norm_mpos);
}

void glfw_resizewindow_callback(GLFWwindow* window, int32 width, int32 height)
{
	nse.event_dispatch()->push<window_resize_event>(ivec2(width,height));
}


#endif
