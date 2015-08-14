#include <iostream>
#include <nsevent.h>
#include <nseventdispatcher.h>
#include <nsengine.h>
#include <nsglfw.h>
#include <nsrendersystem.h>

GLFWwindow * win;

bool glfw_setup(const ivec2 & screendim, nsbool fullscreen, const nsstring & title)
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

void glfw_keypress_callback(GLFWwindow * pWindow, int pKey, int pScancode, int pAction, int pMods)
{
	if (pAction == GLFW_REPEAT)
		return;
	
    bool pressed = pAction == 1;
    switch (pKey)
    {
    case (GLFW_KEY_A) :
        nsengine.eventDispatch()->push<NSKeyEvent>(NSInputMap::Key_A, pressed);
        break;
    case (GLFW_KEY_B) :
        nsengine.eventDispatch()->push<NSKeyEvent>(NSInputMap::Key_B, pressed);
        break;
    case (GLFW_KEY_C) :
        nsengine.eventDispatch()->push<NSKeyEvent>(NSInputMap::Key_C, pressed);
        break;
    case (GLFW_KEY_D) :
        nsengine.eventDispatch()->push<NSKeyEvent>(NSInputMap::Key_D, pressed);
        break;
    case (GLFW_KEY_E) :
        nsengine.eventDispatch()->push<NSKeyEvent>(NSInputMap::Key_E, pressed);
        break;
    case (GLFW_KEY_F) :
        nsengine.eventDispatch()->push<NSKeyEvent>(NSInputMap::Key_F, pressed);
        break;
    case (GLFW_KEY_G) :
        nsengine.eventDispatch()->push<NSKeyEvent>(NSInputMap::Key_G, pressed);
        break;
    case (GLFW_KEY_H) :
        nsengine.eventDispatch()->push<NSKeyEvent>(NSInputMap::Key_H, pressed);
        break;
    case (GLFW_KEY_I) :
        nsengine.eventDispatch()->push<NSKeyEvent>(NSInputMap::Key_I, pressed);
        break;
    case (GLFW_KEY_J) :
        nsengine.eventDispatch()->push<NSKeyEvent>(NSInputMap::Key_J, pressed);
        break;
    case (GLFW_KEY_K) :
        nsengine.eventDispatch()->push<NSKeyEvent>(NSInputMap::Key_K, pressed);
        break;
    case (GLFW_KEY_L) :
        nsengine.eventDispatch()->push<NSKeyEvent>(NSInputMap::Key_L, pressed);
        break;
    case (GLFW_KEY_M) :
        nsengine.eventDispatch()->push<NSKeyEvent>(NSInputMap::Key_M, pressed);
        break;
    case (GLFW_KEY_N) :
        nsengine.eventDispatch()->push<NSKeyEvent>(NSInputMap::Key_N, pressed);
        break;
    case (GLFW_KEY_O) :
        nsengine.eventDispatch()->push<NSKeyEvent>(NSInputMap::Key_O, pressed);
        break;
    case (GLFW_KEY_P) :
        nsengine.eventDispatch()->push<NSKeyEvent>(NSInputMap::Key_P, pressed);
        break;
    case (GLFW_KEY_Q) :
        nsengine.eventDispatch()->push<NSKeyEvent>(NSInputMap::Key_Q, pressed);
        break;
    case (GLFW_KEY_R) :
        nsengine.eventDispatch()->push<NSKeyEvent>(NSInputMap::Key_R, pressed);
        break;
    case (GLFW_KEY_S) :
        nsengine.eventDispatch()->push<NSKeyEvent>(NSInputMap::Key_S, pressed);
        break;
    case (GLFW_KEY_T) :
        nsengine.eventDispatch()->push<NSKeyEvent>(NSInputMap::Key_T, pressed);
        break;
    case (GLFW_KEY_U) :
        nsengine.eventDispatch()->push<NSKeyEvent>(NSInputMap::Key_U, pressed);
        break;
    case (GLFW_KEY_V) :
        nsengine.eventDispatch()->push<NSKeyEvent>(NSInputMap::Key_V, pressed);
        break;
    case (GLFW_KEY_W) :
        nsengine.eventDispatch()->push<NSKeyEvent>(NSInputMap::Key_W, pressed);
        break;
    case (GLFW_KEY_X) :
        nsengine.eventDispatch()->push<NSKeyEvent>(NSInputMap::Key_X, pressed);
        break;
    case (GLFW_KEY_Y) :
        nsengine.eventDispatch()->push<NSKeyEvent>(NSInputMap::Key_Y, pressed);
        break;
    case (GLFW_KEY_Z) :
        nsengine.eventDispatch()->push<NSKeyEvent>(NSInputMap::Key_Z, pressed);
        break;
    case (GLFW_KEY_0) :
        nsengine.eventDispatch()->push<NSKeyEvent>(NSInputMap::Key_0, pressed);
        break;
    case (GLFW_KEY_1) :
        nsengine.eventDispatch()->push<NSKeyEvent>(NSInputMap::Key_1, pressed);
        break;
    case (GLFW_KEY_2) :
        nsengine.eventDispatch()->push<NSKeyEvent>(NSInputMap::Key_2, pressed);
        break;
    case (GLFW_KEY_3) :
        nsengine.eventDispatch()->push<NSKeyEvent>(NSInputMap::Key_3, pressed);
        break;
    case (GLFW_KEY_4) :
        nsengine.eventDispatch()->push<NSKeyEvent>(NSInputMap::Key_4, pressed);
        break;
    case (GLFW_KEY_5) :
        nsengine.eventDispatch()->push<NSKeyEvent>(NSInputMap::Key_5, pressed);
        break;
    case (GLFW_KEY_6) :
        nsengine.eventDispatch()->push<NSKeyEvent>(NSInputMap::Key_6, pressed);
        break;
    case (GLFW_KEY_7) :
        nsengine.eventDispatch()->push<NSKeyEvent>(NSInputMap::Key_7, pressed);
        break;
    case (GLFW_KEY_8) :
        nsengine.eventDispatch()->push<NSKeyEvent>(NSInputMap::Key_8, pressed);
        break;
    case (GLFW_KEY_9) :
        nsengine.eventDispatch()->push<NSKeyEvent>(NSInputMap::Key_9, pressed);
        break;
    case (GLFW_KEY_GRAVE_ACCENT) :
        nsengine.eventDispatch()->push<NSKeyEvent>(NSInputMap::Key_GraveAccent, pressed);
        break;
    case (GLFW_KEY_MINUS) :
        nsengine.eventDispatch()->push<NSKeyEvent>(NSInputMap::Key_Minus, pressed);
        break;
    case (GLFW_KEY_EQUAL) :
        nsengine.eventDispatch()->push<NSKeyEvent>(NSInputMap::Key_Equal, pressed);
        break;
    case (GLFW_KEY_BACKSPACE) :
        nsengine.eventDispatch()->push<NSKeyEvent>(NSInputMap::Key_Backspace, pressed);
        break;
    case (GLFW_KEY_TAB) :
        nsengine.eventDispatch()->push<NSKeyEvent>(NSInputMap::Key_Tab, pressed);
        break;
    case (GLFW_KEY_LEFT_BRACKET) :
        nsengine.eventDispatch()->push<NSKeyEvent>(NSInputMap::Key_LBracket, pressed);
        break;
    case (GLFW_KEY_RIGHT_BRACKET) :
        nsengine.eventDispatch()->push<NSKeyEvent>(NSInputMap::Key_RBracket, pressed);
        break;
    case (GLFW_KEY_BACKSLASH) :
        nsengine.eventDispatch()->push<NSKeyEvent>(NSInputMap::Key_BSlash, pressed);
        break;
    case (GLFW_KEY_CAPS_LOCK) :
        nsengine.eventDispatch()->push<NSKeyEvent>(NSInputMap::Key_CapsLock, pressed);
        break;
    case (GLFW_KEY_SEMICOLON) :
        nsengine.eventDispatch()->push<NSKeyEvent>(NSInputMap::Key_Semicolon, pressed);
        break;
    case (GLFW_KEY_APOSTROPHE) :
        nsengine.eventDispatch()->push<NSKeyEvent>(NSInputMap::Key_Apostrophe, pressed);
        break;
    case (GLFW_KEY_ENTER) :
        nsengine.eventDispatch()->push<NSKeyEvent>(NSInputMap::Key_Enter, pressed);
        break;
    case (GLFW_KEY_LEFT_SHIFT) :
        nsengine.eventDispatch()->push<NSKeyEvent>(NSInputMap::Key_LShift, pressed);
        break;
    case (GLFW_KEY_COMMA) :
        nsengine.eventDispatch()->push<NSKeyEvent>(NSInputMap::Key_Comma, pressed);
        break;
    case (GLFW_KEY_PERIOD) :
        nsengine.eventDispatch()->push<NSKeyEvent>(NSInputMap::Key_Period, pressed);
        break;
    case (GLFW_KEY_SLASH) :
        nsengine.eventDispatch()->push<NSKeyEvent>(NSInputMap::Key_FSlash, pressed);
        break;
    case (GLFW_KEY_RIGHT_SHIFT) :
        nsengine.eventDispatch()->push<NSKeyEvent>(NSInputMap::Key_RShift, pressed);
        break;
    case (GLFW_KEY_LEFT_CONTROL) :
        nsengine.eventDispatch()->push<NSKeyEvent>(NSInputMap::Key_LCtrl, pressed);
        break;
    case (GLFW_KEY_LEFT_SUPER) :
        nsengine.eventDispatch()->push<NSKeyEvent>(NSInputMap::Key_LSuper, pressed);
        break;
    case (GLFW_KEY_LEFT_ALT) :
        nsengine.eventDispatch()->push<NSKeyEvent>(NSInputMap::Key_LAlt, pressed);
        break;
    case (GLFW_KEY_SPACE) :
        nsengine.eventDispatch()->push<NSKeyEvent>(NSInputMap::Key_Space, pressed);
        break;
    case (GLFW_KEY_RIGHT_ALT) :
        nsengine.eventDispatch()->push<NSKeyEvent>(NSInputMap::Key_RAlt, pressed);
        break;
    case (GLFW_KEY_RIGHT_SUPER) :
        nsengine.eventDispatch()->push<NSKeyEvent>(NSInputMap::Key_RSuper, pressed);
        break;
    case (GLFW_KEY_RIGHT_CONTROL) :
        nsengine.eventDispatch()->push<NSKeyEvent>(NSInputMap::Key_RCtrl, pressed);
        break;
    case (GLFW_KEY_LEFT) :
        nsengine.eventDispatch()->push<NSKeyEvent>(NSInputMap::Key_Left, pressed);
        break;
    case (GLFW_KEY_RIGHT) :
        nsengine.eventDispatch()->push<NSKeyEvent>(NSInputMap::Key_Right, pressed);
        break;
    case (GLFW_KEY_UP) :
        nsengine.eventDispatch()->push<NSKeyEvent>(NSInputMap::Key_Up, pressed);
        break;
    case (GLFW_KEY_DOWN) :
        nsengine.eventDispatch()->push<NSKeyEvent>(NSInputMap::Key_Down, pressed);
        break;
    case (GLFW_KEY_ESCAPE) :
        nsengine.eventDispatch()->push<NSKeyEvent>(NSInputMap::Key_Esc, pressed);
        break;
    case (GLFW_KEY_F1) :
        nsengine.eventDispatch()->push<NSKeyEvent>(NSInputMap::Key_F1, pressed);
        break;
    case (GLFW_KEY_F2) :
        nsengine.eventDispatch()->push<NSKeyEvent>(NSInputMap::Key_F2, pressed);
        break;
    case (GLFW_KEY_F3) :
        nsengine.eventDispatch()->push<NSKeyEvent>(NSInputMap::Key_F3, pressed);
        break;
    case (GLFW_KEY_F4) :
        nsengine.eventDispatch()->push<NSKeyEvent>(NSInputMap::Key_F4, pressed);
        break;
    case (GLFW_KEY_F5) :
        nsengine.eventDispatch()->push<NSKeyEvent>(NSInputMap::Key_F5, pressed);
        break;
    case (GLFW_KEY_F6) :
        nsengine.eventDispatch()->push<NSKeyEvent>(NSInputMap::Key_F6, pressed);
        break;
    case (GLFW_KEY_F7) :
        nsengine.eventDispatch()->push<NSKeyEvent>(NSInputMap::Key_F7, pressed);
        break;
    case (GLFW_KEY_F8) :
        nsengine.eventDispatch()->push<NSKeyEvent>(NSInputMap::Key_F8, pressed);
        break;
    case (GLFW_KEY_F9) :
        nsengine.eventDispatch()->push<NSKeyEvent>(NSInputMap::Key_F9, pressed);
        break;
    case (GLFW_KEY_F10) :
        nsengine.eventDispatch()->push<NSKeyEvent>(NSInputMap::Key_F10, pressed);
        break;
    case (GLFW_KEY_F11) :
        nsengine.eventDispatch()->push<NSKeyEvent>(NSInputMap::Key_F11, pressed);
        break;
    case (GLFW_KEY_F12) :
        nsengine.eventDispatch()->push<NSKeyEvent>(NSInputMap::Key_F12, pressed);
        break;
    case (GLFW_KEY_F13) :
        nsengine.eventDispatch()->push<NSKeyEvent>(NSInputMap::Key_F13, pressed);
        break;
    case (GLFW_KEY_F14) :
        nsengine.eventDispatch()->push<NSKeyEvent>(NSInputMap::Key_F14, pressed);
        break;
    case (GLFW_KEY_PRINT_SCREEN) :
        nsengine.eventDispatch()->push<NSKeyEvent>(NSInputMap::Key_PrntScrn, pressed);
        break;
    case (GLFW_KEY_SCROLL_LOCK) :
        nsengine.eventDispatch()->push<NSKeyEvent>(NSInputMap::Key_ScrollLock, pressed);
        break;
    case (GLFW_KEY_PAUSE) :
        nsengine.eventDispatch()->push<NSKeyEvent>(NSInputMap::Key_Pause, pressed);
        break;
    case (GLFW_KEY_INSERT) :
        nsengine.eventDispatch()->push<NSKeyEvent>(NSInputMap::Key_Insert, pressed);
        break;
    case (GLFW_KEY_DELETE) :
        nsengine.eventDispatch()->push<NSKeyEvent>(NSInputMap::Key_Delete, pressed);
        break;
    case (GLFW_KEY_PAGE_UP) :
        nsengine.eventDispatch()->push<NSKeyEvent>(NSInputMap::Key_PGUp, pressed);
        break;
    case (GLFW_KEY_PAGE_DOWN) :
        nsengine.eventDispatch()->push<NSKeyEvent>(NSInputMap::Key_PGDown, pressed);
        break;
    case (GLFW_KEY_HOME) :
        nsengine.eventDispatch()->push<NSKeyEvent>(NSInputMap::Key_Home, pressed);
        break;
    case (GLFW_KEY_END) :
        nsengine.eventDispatch()->push<NSKeyEvent>(NSInputMap::Key_End, pressed);
        break;
    case (GLFW_KEY_NUM_LOCK) :
        nsengine.eventDispatch()->push<NSKeyEvent>(NSInputMap::Key_NumLock, pressed);
        break;
    case (GLFW_KEY_KP_DIVIDE) :
        nsengine.eventDispatch()->push<NSKeyEvent>(NSInputMap::Key_KPDivide, pressed);
        break;
    case (GLFW_KEY_KP_MULTIPLY) :
        nsengine.eventDispatch()->push<NSKeyEvent>(NSInputMap::Key_KPMultiply, pressed);
        break;
    case (GLFW_KEY_KP_SUBTRACT) :
        nsengine.eventDispatch()->push<NSKeyEvent>(NSInputMap::Key_KPSubtract, pressed);
        break;
    case (GLFW_KEY_KP_ADD) :
        nsengine.eventDispatch()->push<NSKeyEvent>(NSInputMap::Key_KPAdd, pressed);
        break;
    case (GLFW_KEY_KP_DECIMAL) :
        nsengine.eventDispatch()->push<NSKeyEvent>(NSInputMap::Key_KPDecimal, pressed);
        break;
    case (GLFW_KEY_KP_ENTER) :
        nsengine.eventDispatch()->push<NSKeyEvent>(NSInputMap::Key_KPEnter, pressed);
        break;
    case (GLFW_KEY_KP_EQUAL) :
        nsengine.eventDispatch()->push<NSKeyEvent>(NSInputMap::Key_KPEqual, pressed);
        break;
    case (GLFW_KEY_KP_0) :
        nsengine.eventDispatch()->push<NSKeyEvent>(NSInputMap::Key_KP0, pressed);
        break;
    case (GLFW_KEY_KP_1) :
        nsengine.eventDispatch()->push<NSKeyEvent>(NSInputMap::Key_KP1, pressed);
        break;
    case (GLFW_KEY_KP_2) :
        nsengine.eventDispatch()->push<NSKeyEvent>(NSInputMap::Key_KP2, pressed);
        break;
    case (GLFW_KEY_KP_3) :
        nsengine.eventDispatch()->push<NSKeyEvent>(NSInputMap::Key_KP3, pressed);
        break;
    case (GLFW_KEY_KP_4) :
        nsengine.eventDispatch()->push<NSKeyEvent>(NSInputMap::Key_KP4, pressed);
        break;
    case (GLFW_KEY_KP_5) :
        nsengine.eventDispatch()->push<NSKeyEvent>(NSInputMap::Key_KP5, pressed);
        break;
    case (GLFW_KEY_KP_6) :
        nsengine.eventDispatch()->push<NSKeyEvent>(NSInputMap::Key_KP6, pressed);
        break;
    case (GLFW_KEY_KP_7) :
        nsengine.eventDispatch()->push<NSKeyEvent>(NSInputMap::Key_KP7, pressed);
        break;
    case (GLFW_KEY_KP_8) :
        nsengine.eventDispatch()->push<NSKeyEvent>(NSInputMap::Key_KP8, pressed);
        break;
    case (GLFW_KEY_KP_9) :
        nsengine.eventDispatch()->push<NSKeyEvent>(NSInputMap::Key_KP9, pressed);
        break;
    default:
        break;
    }
}

void glfw_mousebutton_callback(GLFWwindow * pWindow, int pButton, int pAction, int pMods)
{
	if (pAction == GLFW_REPEAT)
		return;

    double xPos = 0.0, yPos = 0.0; int frameBufX = 0, frameBufY = 0, winX = 0, winY = 0;
    glfwGetCursorPos(pWindow, &xPos, &yPos);
    glfwGetFramebufferSize(pWindow, &frameBufX, &frameBufY);
    glfwGetWindowSize(pWindow, &winX, &winY);

    yPos = winY - yPos; // Switch to opengl coords

    // normalize coords
    yPos /= double(frameBufY);
    xPos /= double(frameBufX);

    // so compilers wont complain turn in to floats explicitly
    nsfloat normXPos = nsfloat(xPos);
    nsfloat normYPos = nsfloat(yPos);

    bool pressed = pAction == 1;

    switch (pButton)
    {
    case (GLFW_MOUSE_BUTTON_LEFT) :
        nsengine.eventDispatch()->push<NSMouseButtonEvent>(NSInputMap::LeftButton, pressed, fvec2(normXPos, normYPos));
        break;
    case (GLFW_MOUSE_BUTTON_RIGHT) :
        nsengine.eventDispatch()->push<NSMouseButtonEvent>(NSInputMap::RightButton, pressed, fvec2(normXPos, normYPos));
        break;
    case (GLFW_MOUSE_BUTTON_MIDDLE) :
        nsengine.eventDispatch()->push<NSMouseButtonEvent>(NSInputMap::MiddleButton, pressed, fvec2(normXPos, normYPos));
        break;
    case (GLFW_MOUSE_BUTTON_4) :
        nsengine.eventDispatch()->push<NSMouseButtonEvent>(NSInputMap::AuxButton1, pressed, fvec2(normXPos, normYPos));
        break;
    case (GLFW_MOUSE_BUTTON_5) :
        nsengine.eventDispatch()->push<NSMouseButtonEvent>(NSInputMap::AuxButton2, pressed, fvec2(normXPos, normYPos));
        break;
    case (GLFW_MOUSE_BUTTON_6) :
        nsengine.eventDispatch()->push<NSMouseButtonEvent>(NSInputMap::AuxButton3, pressed, fvec2(normXPos, normYPos));
        break;
    case (GLFW_MOUSE_BUTTON_7) :
        nsengine.eventDispatch()->push<NSMouseButtonEvent>(NSInputMap::AuxButton4, pressed, fvec2(normXPos, normYPos));
        break;
    default:
        break;
    }
}

void glfw_cursorpos_callback(GLFWwindow * pWindow, double pPosX, double pPosY)
{
    int frameBufX = 0, frameBufY = 0, winX = 0, winY = 0;
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

    int frameBufX = 0, frameBufY = 0, winX = 0, winY = 0;
    glfwGetFramebufferSize(pWindow, &frameBufX, &frameBufY);
    glfwGetWindowSize(pWindow, &winX, &winY);

    yPos = winY - yPos; // Switch to opengl coords

    // normalize coords
    yPos /= double(frameBufY);
    xPos /= double(frameBufX);

    nsengine.eventDispatch()->push<NSMouseScrollEvent>(float(pYOffset), fvec2(float(yPos),float(xPos)));
}

void glfw_resizewindow_callback(GLFWwindow* window, int width, int height)
{
	NSRenderSystem * rs = nsengine.system<NSRenderSystem>();
	if (rs != NULL)
		rs->resizeScreen(ivec2(width,height));
}