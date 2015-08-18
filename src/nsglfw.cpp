#include <iostream>
#include <nsevent.h>
#include <nsevent_dispatcher.h>
#include <nsengine.h>
#include <nsglfw.h>
#include <nsrender_system.h>

GLFWwindow * win;

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
	glfwSetWindowSizeCallback(win, glfw_resizewindow_callback); 
    return true;
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
        nsengine.eventDispatch()->push<NSKeyEvent>(nsinput_map::key_a, pressed);
        break;
    case (GLFW_KEY_B) :
        nsengine.eventDispatch()->push<NSKeyEvent>(nsinput_map::key_b, pressed);
        break;
    case (GLFW_KEY_C) :
        nsengine.eventDispatch()->push<NSKeyEvent>(nsinput_map::key_c, pressed);
        break;
    case (GLFW_KEY_D) :
        nsengine.eventDispatch()->push<NSKeyEvent>(nsinput_map::key_d, pressed);
        break;
    case (GLFW_KEY_E) :
        nsengine.eventDispatch()->push<NSKeyEvent>(nsinput_map::key_e, pressed);
        break;
    case (GLFW_KEY_F) :
        nsengine.eventDispatch()->push<NSKeyEvent>(nsinput_map::key_f, pressed);
        break;
    case (GLFW_KEY_G) :
        nsengine.eventDispatch()->push<NSKeyEvent>(nsinput_map::key_g, pressed);
        break;
    case (GLFW_KEY_H) :
        nsengine.eventDispatch()->push<NSKeyEvent>(nsinput_map::key_h, pressed);
        break;
    case (GLFW_KEY_I) :
        nsengine.eventDispatch()->push<NSKeyEvent>(nsinput_map::key_i, pressed);
        break;
    case (GLFW_KEY_J) :
        nsengine.eventDispatch()->push<NSKeyEvent>(nsinput_map::key_j, pressed);
        break;
    case (GLFW_KEY_K) :
        nsengine.eventDispatch()->push<NSKeyEvent>(nsinput_map::key_k, pressed);
        break;
    case (GLFW_KEY_L) :
        nsengine.eventDispatch()->push<NSKeyEvent>(nsinput_map::key_l, pressed);
        break;
    case (GLFW_KEY_M) :
        nsengine.eventDispatch()->push<NSKeyEvent>(nsinput_map::key_m, pressed);
        break;
    case (GLFW_KEY_N) :
        nsengine.eventDispatch()->push<NSKeyEvent>(nsinput_map::key_n, pressed);
        break;
    case (GLFW_KEY_O) :
        nsengine.eventDispatch()->push<NSKeyEvent>(nsinput_map::key_o, pressed);
        break;
    case (GLFW_KEY_P) :
        nsengine.eventDispatch()->push<NSKeyEvent>(nsinput_map::key_p, pressed);
        break;
    case (GLFW_KEY_Q) :
        nsengine.eventDispatch()->push<NSKeyEvent>(nsinput_map::key_q, pressed);
        break;
    case (GLFW_KEY_R) :
        nsengine.eventDispatch()->push<NSKeyEvent>(nsinput_map::key_r, pressed);
        break;
    case (GLFW_KEY_S) :
        nsengine.eventDispatch()->push<NSKeyEvent>(nsinput_map::key_s, pressed);
        break;
    case (GLFW_KEY_T) :
        nsengine.eventDispatch()->push<NSKeyEvent>(nsinput_map::key_t, pressed);
        break;
    case (GLFW_KEY_U) :
        nsengine.eventDispatch()->push<NSKeyEvent>(nsinput_map::key_u, pressed);
        break;
    case (GLFW_KEY_V) :
        nsengine.eventDispatch()->push<NSKeyEvent>(nsinput_map::key_v, pressed);
        break;
    case (GLFW_KEY_W) :
        nsengine.eventDispatch()->push<NSKeyEvent>(nsinput_map::key_w, pressed);
        break;
    case (GLFW_KEY_X) :
        nsengine.eventDispatch()->push<NSKeyEvent>(nsinput_map::key_x, pressed);
        break;
    case (GLFW_KEY_Y) :
        nsengine.eventDispatch()->push<NSKeyEvent>(nsinput_map::key_y, pressed);
        break;
    case (GLFW_KEY_Z) :
        nsengine.eventDispatch()->push<NSKeyEvent>(nsinput_map::key_z, pressed);
        break;
    case (GLFW_KEY_0) :
        nsengine.eventDispatch()->push<NSKeyEvent>(nsinput_map::key_0, pressed);
        break;
    case (GLFW_KEY_1) :
        nsengine.eventDispatch()->push<NSKeyEvent>(nsinput_map::key_1, pressed);
        break;
    case (GLFW_KEY_2) :
        nsengine.eventDispatch()->push<NSKeyEvent>(nsinput_map::key_2, pressed);
        break;
    case (GLFW_KEY_3) :
        nsengine.eventDispatch()->push<NSKeyEvent>(nsinput_map::key_3, pressed);
        break;
    case (GLFW_KEY_4) :
        nsengine.eventDispatch()->push<NSKeyEvent>(nsinput_map::key_4, pressed);
        break;
    case (GLFW_KEY_5) :
        nsengine.eventDispatch()->push<NSKeyEvent>(nsinput_map::key_5, pressed);
        break;
    case (GLFW_KEY_6) :
        nsengine.eventDispatch()->push<NSKeyEvent>(nsinput_map::key_6, pressed);
        break;
    case (GLFW_KEY_7) :
        nsengine.eventDispatch()->push<NSKeyEvent>(nsinput_map::key_7, pressed);
        break;
    case (GLFW_KEY_8) :
        nsengine.eventDispatch()->push<NSKeyEvent>(nsinput_map::key_8, pressed);
        break;
    case (GLFW_KEY_9) :
        nsengine.eventDispatch()->push<NSKeyEvent>(nsinput_map::key_9, pressed);
        break;
    case (GLFW_KEY_GRAVE_ACCENT) :
        nsengine.eventDispatch()->push<NSKeyEvent>(nsinput_map::key_grave_accent, pressed);
        break;
    case (GLFW_KEY_MINUS) :
        nsengine.eventDispatch()->push<NSKeyEvent>(nsinput_map::key_minus, pressed);
        break;
    case (GLFW_KEY_EQUAL) :
        nsengine.eventDispatch()->push<NSKeyEvent>(nsinput_map::key_equal, pressed);
        break;
    case (GLFW_KEY_BACKSPACE) :
        nsengine.eventDispatch()->push<NSKeyEvent>(nsinput_map::key_backspace, pressed);
        break;
    case (GLFW_KEY_TAB) :
        nsengine.eventDispatch()->push<NSKeyEvent>(nsinput_map::key_tab, pressed);
        break;
    case (GLFW_KEY_LEFT_BRACKET) :
        nsengine.eventDispatch()->push<NSKeyEvent>(nsinput_map::key_lbracket, pressed);
        break;
    case (GLFW_KEY_RIGHT_BRACKET) :
        nsengine.eventDispatch()->push<NSKeyEvent>(nsinput_map::key_rbracket, pressed);
        break;
    case (GLFW_KEY_BACKSLASH) :
        nsengine.eventDispatch()->push<NSKeyEvent>(nsinput_map::key_backslash, pressed);
        break;
    case (GLFW_KEY_CAPS_LOCK) :
        nsengine.eventDispatch()->push<NSKeyEvent>(nsinput_map::key_capslock, pressed);
        break;
    case (GLFW_KEY_SEMICOLON) :
        nsengine.eventDispatch()->push<NSKeyEvent>(nsinput_map::key_semicolon, pressed);
        break;
    case (GLFW_KEY_APOSTROPHE) :
        nsengine.eventDispatch()->push<NSKeyEvent>(nsinput_map::key_apostrophe, pressed);
        break;
    case (GLFW_KEY_ENTER) :
        nsengine.eventDispatch()->push<NSKeyEvent>(nsinput_map::key_enter, pressed);
        break;
    case (GLFW_KEY_LEFT_SHIFT) :
        nsengine.eventDispatch()->push<NSKeyEvent>(nsinput_map::key_lshift, pressed);
        break;
    case (GLFW_KEY_COMMA) :
        nsengine.eventDispatch()->push<NSKeyEvent>(nsinput_map::key_comma, pressed);
        break;
    case (GLFW_KEY_PERIOD) :
        nsengine.eventDispatch()->push<NSKeyEvent>(nsinput_map::key_period, pressed);
        break;
    case (GLFW_KEY_SLASH) :
        nsengine.eventDispatch()->push<NSKeyEvent>(nsinput_map::key_forwardslash, pressed);
        break;
    case (GLFW_KEY_RIGHT_SHIFT) :
        nsengine.eventDispatch()->push<NSKeyEvent>(nsinput_map::key_rshift, pressed);
        break;
    case (GLFW_KEY_LEFT_CONTROL) :
        nsengine.eventDispatch()->push<NSKeyEvent>(nsinput_map::key_lctrl, pressed);
        break;
    case (GLFW_KEY_LEFT_SUPER) :
        nsengine.eventDispatch()->push<NSKeyEvent>(nsinput_map::key_lsuper, pressed);
        break;
    case (GLFW_KEY_LEFT_ALT) :
        nsengine.eventDispatch()->push<NSKeyEvent>(nsinput_map::key_lalt, pressed);
        break;
    case (GLFW_KEY_SPACE) :
        nsengine.eventDispatch()->push<NSKeyEvent>(nsinput_map::key_space, pressed);
        break;
    case (GLFW_KEY_RIGHT_ALT) :
        nsengine.eventDispatch()->push<NSKeyEvent>(nsinput_map::key_ralt, pressed);
        break;
    case (GLFW_KEY_RIGHT_SUPER) :
        nsengine.eventDispatch()->push<NSKeyEvent>(nsinput_map::key_rsuper, pressed);
        break;
    case (GLFW_KEY_RIGHT_CONTROL) :
        nsengine.eventDispatch()->push<NSKeyEvent>(nsinput_map::key_rctrl, pressed);
        break;
    case (GLFW_KEY_LEFT) :
        nsengine.eventDispatch()->push<NSKeyEvent>(nsinput_map::key_left, pressed);
        break;
    case (GLFW_KEY_RIGHT) :
        nsengine.eventDispatch()->push<NSKeyEvent>(nsinput_map::key_right, pressed);
        break;
    case (GLFW_KEY_UP) :
        nsengine.eventDispatch()->push<NSKeyEvent>(nsinput_map::key_up, pressed);
        break;
    case (GLFW_KEY_DOWN) :
        nsengine.eventDispatch()->push<NSKeyEvent>(nsinput_map::key_down, pressed);
        break;
    case (GLFW_KEY_ESCAPE) :
        nsengine.eventDispatch()->push<NSKeyEvent>(nsinput_map::key_esc, pressed);
        break;
    case (GLFW_KEY_F1) :
        nsengine.eventDispatch()->push<NSKeyEvent>(nsinput_map::key_f1, pressed);
        break;
    case (GLFW_KEY_F2) :
        nsengine.eventDispatch()->push<NSKeyEvent>(nsinput_map::key_f2, pressed);
        break;
    case (GLFW_KEY_F3) :
        nsengine.eventDispatch()->push<NSKeyEvent>(nsinput_map::key_f3, pressed);
        break;
    case (GLFW_KEY_F4) :
        nsengine.eventDispatch()->push<NSKeyEvent>(nsinput_map::key_f4, pressed);
        break;
    case (GLFW_KEY_F5) :
        nsengine.eventDispatch()->push<NSKeyEvent>(nsinput_map::key_f5, pressed);
        break;
    case (GLFW_KEY_F6) :
        nsengine.eventDispatch()->push<NSKeyEvent>(nsinput_map::key_f6, pressed);
        break;
    case (GLFW_KEY_F7) :
        nsengine.eventDispatch()->push<NSKeyEvent>(nsinput_map::key_f7, pressed);
        break;
    case (GLFW_KEY_F8) :
        nsengine.eventDispatch()->push<NSKeyEvent>(nsinput_map::key_f8, pressed);
        break;
    case (GLFW_KEY_F9) :
        nsengine.eventDispatch()->push<NSKeyEvent>(nsinput_map::key_f9, pressed);
        break;
    case (GLFW_KEY_F10) :
        nsengine.eventDispatch()->push<NSKeyEvent>(nsinput_map::key_f10, pressed);
        break;
    case (GLFW_KEY_F11) :
        nsengine.eventDispatch()->push<NSKeyEvent>(nsinput_map::key_f11, pressed);
        break;
    case (GLFW_KEY_F12) :
        nsengine.eventDispatch()->push<NSKeyEvent>(nsinput_map::key_f12, pressed);
        break;
    case (GLFW_KEY_F13) :
        nsengine.eventDispatch()->push<NSKeyEvent>(nsinput_map::key_f13, pressed);
        break;
    case (GLFW_KEY_F14) :
        nsengine.eventDispatch()->push<NSKeyEvent>(nsinput_map::key_f14, pressed);
        break;
    case (GLFW_KEY_PRINT_SCREEN) :
        nsengine.eventDispatch()->push<NSKeyEvent>(nsinput_map::key_print_screen, pressed);
        break;
    case (GLFW_KEY_SCROLL_LOCK) :
        nsengine.eventDispatch()->push<NSKeyEvent>(nsinput_map::key_scroll_lock, pressed);
        break;
    case (GLFW_KEY_PAUSE) :
        nsengine.eventDispatch()->push<NSKeyEvent>(nsinput_map::key_pause, pressed);
        break;
    case (GLFW_KEY_INSERT) :
        nsengine.eventDispatch()->push<NSKeyEvent>(nsinput_map::key_insert, pressed);
        break;
    case (GLFW_KEY_DELETE) :
        nsengine.eventDispatch()->push<NSKeyEvent>(nsinput_map::key_delete, pressed);
        break;
    case (GLFW_KEY_PAGE_UP) :
        nsengine.eventDispatch()->push<NSKeyEvent>(nsinput_map::key_page_up, pressed);
        break;
    case (GLFW_KEY_PAGE_DOWN) :
        nsengine.eventDispatch()->push<NSKeyEvent>(nsinput_map::key_page_down, pressed);
        break;
    case (GLFW_KEY_HOME) :
        nsengine.eventDispatch()->push<NSKeyEvent>(nsinput_map::key_home, pressed);
        break;
    case (GLFW_KEY_END) :
        nsengine.eventDispatch()->push<NSKeyEvent>(nsinput_map::key_end, pressed);
        break;
    case (GLFW_KEY_NUM_LOCK) :
        nsengine.eventDispatch()->push<NSKeyEvent>(nsinput_map::key_numlock, pressed);
        break;
    case (GLFW_KEY_KP_DIVIDE) :
        nsengine.eventDispatch()->push<NSKeyEvent>(nsinput_map::key_keypad_divide, pressed);
        break;
    case (GLFW_KEY_KP_MULTIPLY) :
        nsengine.eventDispatch()->push<NSKeyEvent>(nsinput_map::key_keypad_multiply, pressed);
        break;
    case (GLFW_KEY_KP_SUBTRACT) :
        nsengine.eventDispatch()->push<NSKeyEvent>(nsinput_map::key_keypad_subtract, pressed);
        break;
    case (GLFW_KEY_KP_ADD) :
        nsengine.eventDispatch()->push<NSKeyEvent>(nsinput_map::key_keypad_add, pressed);
        break;
    case (GLFW_KEY_KP_DECIMAL) :
        nsengine.eventDispatch()->push<NSKeyEvent>(nsinput_map::key_keypad_decimal, pressed);
        break;
    case (GLFW_KEY_KP_ENTER) :
        nsengine.eventDispatch()->push<NSKeyEvent>(nsinput_map::key_keypad_enter, pressed);
        break;
    case (GLFW_KEY_KP_EQUAL) :
        nsengine.eventDispatch()->push<NSKeyEvent>(nsinput_map::key_keypad_equal, pressed);
        break;
    case (GLFW_KEY_KP_0) :
        nsengine.eventDispatch()->push<NSKeyEvent>(nsinput_map::key_keypad_0, pressed);
        break;
    case (GLFW_KEY_KP_1) :
        nsengine.eventDispatch()->push<NSKeyEvent>(nsinput_map::key_keypad_1, pressed);
        break;
    case (GLFW_KEY_KP_2) :
        nsengine.eventDispatch()->push<NSKeyEvent>(nsinput_map::key_keypad_2, pressed);
        break;
    case (GLFW_KEY_KP_3) :
        nsengine.eventDispatch()->push<NSKeyEvent>(nsinput_map::key_keypad_3, pressed);
        break;
    case (GLFW_KEY_KP_4) :
        nsengine.eventDispatch()->push<NSKeyEvent>(nsinput_map::key_keypad_4, pressed);
        break;
    case (GLFW_KEY_KP_5) :
        nsengine.eventDispatch()->push<NSKeyEvent>(nsinput_map::key_keypad_5, pressed);
        break;
    case (GLFW_KEY_KP_6) :
        nsengine.eventDispatch()->push<NSKeyEvent>(nsinput_map::key_keypad_6, pressed);
        break;
    case (GLFW_KEY_KP_7) :
        nsengine.eventDispatch()->push<NSKeyEvent>(nsinput_map::key_keypad_7, pressed);
        break;
    case (GLFW_KEY_KP_8) :
        nsengine.eventDispatch()->push<NSKeyEvent>(nsinput_map::key_keypad_8, pressed);
        break;
    case (GLFW_KEY_KP_9) :
        nsengine.eventDispatch()->push<NSKeyEvent>(nsinput_map::key_keypad_9, pressed);
        break;
    default:
        break;
    }
}

void glfw_mousebutton_callback(GLFWwindow * pWindow, int32 pButton, int32 pAction, int32 pMods)
{
	if (pAction == GLFW_REPEAT)
		return;

    double xPos = 0.0, yPos = 0.0; int32 frameBufX = 0, frameBufY = 0, winX = 0, winY = 0;
    glfwGetCursorPos(pWindow, &xPos, &yPos);
    glfwGetFramebufferSize(pWindow, &frameBufX, &frameBufY);
    glfwGetWindowSize(pWindow, &winX, &winY);

    yPos = winY - yPos; // Switch to opengl coords

    // normalize coords
    yPos /= double(frameBufY);
    xPos /= double(frameBufX);

    // so compilers wont complain turn in to floats explicitly
    float normXPos = float(xPos);
    float normYPos = float(yPos);

    bool pressed = pAction == 1;

    switch (pButton)
    {
    case (GLFW_MOUSE_BUTTON_LEFT) :
        nsengine.eventDispatch()->push<NSMouseButtonEvent>(nsinput_map::left_button, pressed, fvec2(normXPos, normYPos));
        break;
    case (GLFW_MOUSE_BUTTON_RIGHT) :
        nsengine.eventDispatch()->push<NSMouseButtonEvent>(nsinput_map::right_button, pressed, fvec2(normXPos, normYPos));
        break;
    case (GLFW_MOUSE_BUTTON_MIDDLE) :
        nsengine.eventDispatch()->push<NSMouseButtonEvent>(nsinput_map::middle_button, pressed, fvec2(normXPos, normYPos));
        break;
    case (GLFW_MOUSE_BUTTON_4) :
        nsengine.eventDispatch()->push<NSMouseButtonEvent>(nsinput_map::aux_button_1, pressed, fvec2(normXPos, normYPos));
        break;
    case (GLFW_MOUSE_BUTTON_5) :
        nsengine.eventDispatch()->push<NSMouseButtonEvent>(nsinput_map::aux_button_2, pressed, fvec2(normXPos, normYPos));
        break;
    case (GLFW_MOUSE_BUTTON_6) :
        nsengine.eventDispatch()->push<NSMouseButtonEvent>(nsinput_map::aux_button_3, pressed, fvec2(normXPos, normYPos));
        break;
    case (GLFW_MOUSE_BUTTON_7) :
        nsengine.eventDispatch()->push<NSMouseButtonEvent>(nsinput_map::aux_button_4, pressed, fvec2(normXPos, normYPos));
        break;
    default:
        break;
    }
}

void glfw_cursorpos_callback(GLFWwindow * pWindow, double pPosX, double pPosY)
{
    int32 frameBufX = 0, frameBufY = 0, winX = 0, winY = 0;
    glfwGetFramebufferSize(pWindow, &frameBufX, &frameBufY);
    glfwGetWindowSize(pWindow, &winX, &winY);

    pPosY = winY - pPosY; // Switch to opengl coords

    // normalize coords
    pPosY /= double(frameBufY);
    pPosX /= double(frameBufX);


    nsengine.eventDispatch()->push<NSMouseMoveEvent>(fvec2(float(pPosX),float(pPosY)));
}

void glfw_scroll_callback(GLFWwindow * pWindow, double pXOffset, double pYOffset)
{
    double xPos = 0.0, yPos = 0.0;
    glfwGetCursorPos(pWindow, &xPos, &yPos);

    int32 frameBufX = 0, frameBufY = 0, winX = 0, winY = 0;
    glfwGetFramebufferSize(pWindow, &frameBufX, &frameBufY);
    glfwGetWindowSize(pWindow, &winX, &winY);

    yPos = winY - yPos; // Switch to opengl coords

    // normalize coords
    yPos /= double(frameBufY);
    xPos /= double(frameBufX);

    nsengine.eventDispatch()->push<NSMouseScrollEvent>(float(pYOffset), fvec2(float(yPos),float(xPos)));
}

void glfw_resizewindow_callback(GLFWwindow* window, int32 width, int32 height)
{
	nsrender_system * rs = nsengine.system<nsrender_system>();
	if (rs != NULL)
		rs->resize_screen(ivec2(width,height));
}
