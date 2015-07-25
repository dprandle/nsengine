#ifdef GLFW
#include <nsengine.h>
#include <GLFW\glfw3.h>
#include <nscamcomp.h>
#include <nsentity.h>
#include <nsrendersystem.h>

#include <nstimer.h>

struct GLFWwindow;
void GLFWKeyCallback(GLFWwindow * pWindow, int pKey, int pScancode, int pAction, int pMods);
void GLFWMouseButtonCallback(GLFWwindow * pWindow, int pButton, int pAction, int pMods);
void GLFWCursorPosCallback(GLFWwindow * pWindow, double pPosX, double pPosY);
void GLFWScrollCallback(GLFWwindow * pWindow, double pXOffset, double pYOffset);

void GLFWKeyCallback(GLFWwindow * pWindow, int pKey, int pScancode, int pAction, int pMods)
{
	if (pAction == GLFW_PRESS)
	{
		switch (pKey)
		{
		case (GLFW_KEY_A) :
			nsengine.input()->keyPress(NSInputManager::Key_A);
			break;
		case (GLFW_KEY_B) :
			nsengine.input()->keyPress(NSInputManager::Key_B);
			break;
		case (GLFW_KEY_C) :
			nsengine.input()->keyPress(NSInputManager::Key_C);
			break;
		case (GLFW_KEY_D) :
			nsengine.input()->keyPress(NSInputManager::Key_D);
			break;
		case (GLFW_KEY_E) :
			nsengine.input()->keyPress(NSInputManager::Key_E);
			break;
		case (GLFW_KEY_F) :
			nsengine.input()->keyPress(NSInputManager::Key_F);
			break;
		case (GLFW_KEY_G) :
			nsengine.input()->keyPress(NSInputManager::Key_G);
			break;
		case (GLFW_KEY_H) :
			nsengine.input()->keyPress(NSInputManager::Key_H);
			break;
		case (GLFW_KEY_I) :
			nsengine.input()->keyPress(NSInputManager::Key_I);
			break;
		case (GLFW_KEY_J) :
			nsengine.input()->keyPress(NSInputManager::Key_J);
			break;
		case (GLFW_KEY_K) :
			nsengine.input()->keyPress(NSInputManager::Key_K);
			break;
		case (GLFW_KEY_L) :
			nsengine.input()->keyPress(NSInputManager::Key_L);
			break;
		case (GLFW_KEY_M) :
			nsengine.input()->keyPress(NSInputManager::Key_M);
			break;
		case (GLFW_KEY_N) :
			nsengine.input()->keyPress(NSInputManager::Key_N);
			break;
		case (GLFW_KEY_O) :
			nsengine.input()->keyPress(NSInputManager::Key_O);
			break;
		case (GLFW_KEY_P) :
			nsengine.input()->keyPress(NSInputManager::Key_P);
			break;
		case (GLFW_KEY_Q) :
			nsengine.input()->keyPress(NSInputManager::Key_Q);
			break;
		case (GLFW_KEY_R) :
			nsengine.input()->keyPress(NSInputManager::Key_R);
			break;
		case (GLFW_KEY_S) :
			nsengine.input()->keyPress(NSInputManager::Key_S);
			break;
		case (GLFW_KEY_T) :
			nsengine.input()->keyPress(NSInputManager::Key_T);
			break;
		case (GLFW_KEY_U) :
			nsengine.input()->keyPress(NSInputManager::Key_U);
			break;
		case (GLFW_KEY_V) :
			nsengine.input()->keyPress(NSInputManager::Key_V);
			break;
		case (GLFW_KEY_W) :
			nsengine.input()->keyPress(NSInputManager::Key_W);
			break;
		case (GLFW_KEY_X) :
			nsengine.input()->keyPress(NSInputManager::Key_X);
			break;
		case (GLFW_KEY_Y) :
			nsengine.input()->keyPress(NSInputManager::Key_Y);
			break;
		case (GLFW_KEY_Z) :
			nsengine.input()->keyPress(NSInputManager::Key_Z);
			break;
		case (GLFW_KEY_0) :
			nsengine.input()->keyPress(NSInputManager::Key_0);
			break;
		case (GLFW_KEY_1) :
			nsengine.input()->keyPress(NSInputManager::Key_1);
			break;
		case (GLFW_KEY_2) :
			nsengine.input()->keyPress(NSInputManager::Key_2);
			break;
		case (GLFW_KEY_3) :
			nsengine.input()->keyPress(NSInputManager::Key_3);
			break;
		case (GLFW_KEY_4) :
			nsengine.input()->keyPress(NSInputManager::Key_4);
			break;
		case (GLFW_KEY_5) :
			nsengine.input()->keyPress(NSInputManager::Key_5);
			break;
		case (GLFW_KEY_6) :
			nsengine.input()->keyPress(NSInputManager::Key_6);
			break;
		case (GLFW_KEY_7) :
			nsengine.input()->keyPress(NSInputManager::Key_7);
			break;
		case (GLFW_KEY_8) :
			nsengine.input()->keyPress(NSInputManager::Key_8);
			break;
		case (GLFW_KEY_9) :
			nsengine.input()->keyPress(NSInputManager::Key_9);
			break;
		case (GLFW_KEY_GRAVE_ACCENT) :
			nsengine.input()->keyPress(NSInputManager::Key_GraveAccent);
			break;
		case (GLFW_KEY_MINUS) :
			nsengine.input()->keyPress(NSInputManager::Key_Minus);
			break;
		case (GLFW_KEY_EQUAL) :
			nsengine.input()->keyPress(NSInputManager::Key_Equal);
			break;
		case (GLFW_KEY_BACKSPACE) :
			nsengine.input()->keyPress(NSInputManager::Key_Backspace);
			break;
		case (GLFW_KEY_TAB) :
			nsengine.input()->keyPress(NSInputManager::Key_Tab);
			break;
		case (GLFW_KEY_LEFT_BRACKET) :
			nsengine.input()->keyPress(NSInputManager::Key_LBracket);
			break;
		case (GLFW_KEY_RIGHT_BRACKET) :
			nsengine.input()->keyPress(NSInputManager::Key_RBracket);
			break;
		case (GLFW_KEY_BACKSLASH) :
			nsengine.input()->keyPress(NSInputManager::Key_BSlash);
			break;
		case (GLFW_KEY_CAPS_LOCK) :
			nsengine.input()->keyPress(NSInputManager::Key_CapsLock);
			break;
		case (GLFW_KEY_SEMICOLON) :
			nsengine.input()->keyPress(NSInputManager::Key_Semicolon);
			break;
		case (GLFW_KEY_APOSTROPHE) :
			nsengine.input()->keyPress(NSInputManager::Key_Apostrophe);
			break;
		case (GLFW_KEY_ENTER) :
			nsengine.input()->keyPress(NSInputManager::Key_Enter);
			break;
		case (GLFW_KEY_LEFT_SHIFT) :
			nsengine.input()->keyPress(NSInputManager::Key_LShift);
			break;
		case (GLFW_KEY_COMMA) :
			nsengine.input()->keyPress(NSInputManager::Key_Comma);
			break;
		case (GLFW_KEY_PERIOD) :
			nsengine.input()->keyPress(NSInputManager::Key_Period);
			break;
		case (GLFW_KEY_SLASH) :
			nsengine.input()->keyPress(NSInputManager::Key_FSlash);
			break;
		case (GLFW_KEY_RIGHT_SHIFT) :
			nsengine.input()->keyPress(NSInputManager::Key_RShift);
			break;
		case (GLFW_KEY_LEFT_CONTROL) :
			nsengine.input()->keyPress(NSInputManager::Key_LCtrl);
			break;
		case (GLFW_KEY_LEFT_SUPER) :
			nsengine.input()->keyPress(NSInputManager::Key_LSuper);
			break;
		case (GLFW_KEY_LEFT_ALT) :
			nsengine.input()->keyPress(NSInputManager::Key_LAlt);
			break;
		case (GLFW_KEY_SPACE) :
			nsengine.input()->keyPress(NSInputManager::Key_Space);
			break;
		case (GLFW_KEY_RIGHT_ALT) :
			nsengine.input()->keyPress(NSInputManager::Key_RAlt);
			break;
		case (GLFW_KEY_RIGHT_SUPER) :
			nsengine.input()->keyPress(NSInputManager::Key_RSuper);
			break;
		case (GLFW_KEY_RIGHT_CONTROL) :
			nsengine.input()->keyPress(NSInputManager::Key_RCtrl);
			break;
		case (GLFW_KEY_LEFT) :
			nsengine.input()->keyPress(NSInputManager::Key_Left);
			break;
		case (GLFW_KEY_RIGHT) :
			nsengine.input()->keyPress(NSInputManager::Key_Right);
			break;
		case (GLFW_KEY_UP) :
			nsengine.input()->keyPress(NSInputManager::Key_Up);
			break;
		case (GLFW_KEY_DOWN) :
			nsengine.input()->keyPress(NSInputManager::Key_Down);
			break;
		case (GLFW_KEY_ESCAPE) :
			nsengine.input()->keyPress(NSInputManager::Key_Esc);
			break;
		case (GLFW_KEY_F1) :
			nsengine.input()->keyPress(NSInputManager::Key_F1);
			break;
		case (GLFW_KEY_F2) :
			nsengine.input()->keyPress(NSInputManager::Key_F2);
			break;
		case (GLFW_KEY_F3) :
			nsengine.input()->keyPress(NSInputManager::Key_F3);
			break;
		case (GLFW_KEY_F4) :
			nsengine.input()->keyPress(NSInputManager::Key_F4);
			break;
		case (GLFW_KEY_F5) :
			nsengine.input()->keyPress(NSInputManager::Key_F5);
			break;
		case (GLFW_KEY_F6) :
			nsengine.input()->keyPress(NSInputManager::Key_F6);
			break;
		case (GLFW_KEY_F7) :
			nsengine.input()->keyPress(NSInputManager::Key_F7);
			break;
		case (GLFW_KEY_F8) :
			nsengine.input()->keyPress(NSInputManager::Key_F8);
			break;
		case (GLFW_KEY_F9) :
			nsengine.input()->keyPress(NSInputManager::Key_F9);
			break;
		case (GLFW_KEY_F10) :
			nsengine.input()->keyPress(NSInputManager::Key_F10);
			break;
		case (GLFW_KEY_F11) :
			nsengine.input()->keyPress(NSInputManager::Key_F11);
			break;
		case (GLFW_KEY_F12) :
			nsengine.input()->keyPress(NSInputManager::Key_F12);
			break;
		case (GLFW_KEY_F13) :
			nsengine.input()->keyPress(NSInputManager::Key_F13);
			break;
		case (GLFW_KEY_F14) :
			nsengine.input()->keyPress(NSInputManager::Key_F14);
			break;
		case (GLFW_KEY_PRINT_SCREEN) :
			nsengine.input()->keyPress(NSInputManager::Key_PrntScrn);
			break;
		case (GLFW_KEY_SCROLL_LOCK) :
			nsengine.input()->keyPress(NSInputManager::Key_ScrollLock);
			break;
		case (GLFW_KEY_PAUSE) :
			nsengine.input()->keyPress(NSInputManager::Key_Pause);
			break;
		case (GLFW_KEY_INSERT) :
			nsengine.input()->keyPress(NSInputManager::Key_Insert);
			break;
		case (GLFW_KEY_DELETE) :
			nsengine.input()->keyPress(NSInputManager::Key_Delete);
			break;
		case (GLFW_KEY_PAGE_UP) :
			nsengine.input()->keyPress(NSInputManager::Key_PGUp);
			break;
		case (GLFW_KEY_PAGE_DOWN) :
			nsengine.input()->keyPress(NSInputManager::Key_PGDown);
			break;
		case (GLFW_KEY_HOME) :
			nsengine.input()->keyPress(NSInputManager::Key_Home);
			break;
		case (GLFW_KEY_END) :
			nsengine.input()->keyPress(NSInputManager::Key_End);
			break;
		case (GLFW_KEY_NUM_LOCK) :
			nsengine.input()->keyPress(NSInputManager::Key_NumLock);
			break;
		case (GLFW_KEY_KP_DIVIDE) :
			nsengine.input()->keyPress(NSInputManager::Key_KPDivide);
			break;
		case (GLFW_KEY_KP_MULTIPLY) :
			nsengine.input()->keyPress(NSInputManager::Key_KPMultiply);
			break;
		case (GLFW_KEY_KP_SUBTRACT) :
			nsengine.input()->keyPress(NSInputManager::Key_KPSubtract);
			break;
		case (GLFW_KEY_KP_ADD) :
			nsengine.input()->keyPress(NSInputManager::Key_KPAdd);
			break;
		case (GLFW_KEY_KP_DECIMAL) :
			nsengine.input()->keyPress(NSInputManager::Key_KPDecimal);
			break;
		case (GLFW_KEY_KP_ENTER) :
			nsengine.input()->keyPress(NSInputManager::Key_KPEnter);
			break;
		case (GLFW_KEY_KP_EQUAL) :
			nsengine.input()->keyPress(NSInputManager::Key_KPEqual);
			break;
		case (GLFW_KEY_KP_0) :
			nsengine.input()->keyPress(NSInputManager::Key_KP0);
			break;
		case (GLFW_KEY_KP_1) :
			nsengine.input()->keyPress(NSInputManager::Key_KP1);
			break;
		case (GLFW_KEY_KP_2) :
			nsengine.input()->keyPress(NSInputManager::Key_KP2);
			break;
		case (GLFW_KEY_KP_3) :
			nsengine.input()->keyPress(NSInputManager::Key_KP3);
			break;
		case (GLFW_KEY_KP_4) :
			nsengine.input()->keyPress(NSInputManager::Key_KP4);
			break;
		case (GLFW_KEY_KP_5) :
			nsengine.input()->keyPress(NSInputManager::Key_KP5);
			break;
		case (GLFW_KEY_KP_6) :
			nsengine.input()->keyPress(NSInputManager::Key_KP6);
			break;
		case (GLFW_KEY_KP_7) :
			nsengine.input()->keyPress(NSInputManager::Key_KP7);
			break;
		case (GLFW_KEY_KP_8) :
			nsengine.input()->keyPress(NSInputManager::Key_KP8);
			break;
		case (GLFW_KEY_KP_9) :
			nsengine.input()->keyPress(NSInputManager::Key_KP9);
			break;
		default:
			nsengine.input()->keyPress(NSInputManager::Key_None);
		}
	}
	else if (pAction == GLFW_RELEASE)
	{
		switch (pKey)
		{
		case (GLFW_KEY_A) :
			nsengine.input()->keyRelease(NSInputManager::Key_A);
			break;
		case (GLFW_KEY_B) :
			nsengine.input()->keyRelease(NSInputManager::Key_B);
			break;
		case (GLFW_KEY_C) :
			nsengine.input()->keyRelease(NSInputManager::Key_C);
			break;
		case (GLFW_KEY_D) :
			nsengine.input()->keyRelease(NSInputManager::Key_D);
			break;
		case (GLFW_KEY_E) :
			nsengine.input()->keyRelease(NSInputManager::Key_E);
			break;
		case (GLFW_KEY_F) :
			nsengine.input()->keyRelease(NSInputManager::Key_F);
			break;
		case (GLFW_KEY_G) :
			nsengine.input()->keyRelease(NSInputManager::Key_G);
			break;
		case (GLFW_KEY_H) :
			nsengine.input()->keyRelease(NSInputManager::Key_H);
			break;
		case (GLFW_KEY_I) :
			nsengine.input()->keyRelease(NSInputManager::Key_I);
			break;
		case (GLFW_KEY_J) :
			nsengine.input()->keyRelease(NSInputManager::Key_J);
			break;
		case (GLFW_KEY_K) :
			nsengine.input()->keyRelease(NSInputManager::Key_K);
			break;
		case (GLFW_KEY_L) :
			nsengine.input()->keyRelease(NSInputManager::Key_L);
			break;
		case (GLFW_KEY_M) :
			nsengine.input()->keyRelease(NSInputManager::Key_M);
			break;
		case (GLFW_KEY_N) :
			nsengine.input()->keyRelease(NSInputManager::Key_N);
			break;
		case (GLFW_KEY_O) :
			nsengine.input()->keyRelease(NSInputManager::Key_O);
			break;
		case (GLFW_KEY_P) :
			nsengine.input()->keyRelease(NSInputManager::Key_P);
			break;
		case (GLFW_KEY_Q) :
			nsengine.input()->keyRelease(NSInputManager::Key_Q);
			break;
		case (GLFW_KEY_R) :
			nsengine.input()->keyRelease(NSInputManager::Key_R);
			break;
		case (GLFW_KEY_S) :
			nsengine.input()->keyRelease(NSInputManager::Key_S);
			break;
		case (GLFW_KEY_T) :
			nsengine.input()->keyRelease(NSInputManager::Key_T);
			break;
		case (GLFW_KEY_U) :
			nsengine.input()->keyRelease(NSInputManager::Key_U);
			break;
		case (GLFW_KEY_V) :
			nsengine.input()->keyRelease(NSInputManager::Key_V);
			break;
		case (GLFW_KEY_W) :
			nsengine.input()->keyRelease(NSInputManager::Key_W);
			break;
		case (GLFW_KEY_X) :
			nsengine.input()->keyRelease(NSInputManager::Key_X);
			break;
		case (GLFW_KEY_Y) :
			nsengine.input()->keyRelease(NSInputManager::Key_Y);
			break;
		case (GLFW_KEY_Z) :
			nsengine.input()->keyRelease(NSInputManager::Key_Z);
			break;
		case (GLFW_KEY_0) :
			nsengine.input()->keyRelease(NSInputManager::Key_0);
			break;
		case (GLFW_KEY_1) :
			nsengine.input()->keyRelease(NSInputManager::Key_1);
			break;
		case (GLFW_KEY_2) :
			nsengine.input()->keyRelease(NSInputManager::Key_2);
			break;
		case (GLFW_KEY_3) :
			nsengine.input()->keyRelease(NSInputManager::Key_3);
			break;
		case (GLFW_KEY_4) :
			nsengine.input()->keyRelease(NSInputManager::Key_4);
			break;
		case (GLFW_KEY_5) :
			nsengine.input()->keyRelease(NSInputManager::Key_5);
			break;
		case (GLFW_KEY_6) :
			nsengine.input()->keyRelease(NSInputManager::Key_6);
			break;
		case (GLFW_KEY_7) :
			nsengine.input()->keyRelease(NSInputManager::Key_7);
			break;
		case (GLFW_KEY_8) :
			nsengine.input()->keyRelease(NSInputManager::Key_8);
			break;
		case (GLFW_KEY_9) :
			nsengine.input()->keyRelease(NSInputManager::Key_9);
			break;
		case (GLFW_KEY_GRAVE_ACCENT) :
			nsengine.input()->keyRelease(NSInputManager::Key_GraveAccent);
			break;
		case (GLFW_KEY_MINUS) :
			nsengine.input()->keyRelease(NSInputManager::Key_Minus);
			break;
		case (GLFW_KEY_EQUAL) :
			nsengine.input()->keyRelease(NSInputManager::Key_Equal);
			break;
		case (GLFW_KEY_BACKSPACE) :
			nsengine.input()->keyRelease(NSInputManager::Key_Backspace);
			break;
		case (GLFW_KEY_TAB) :
			nsengine.input()->keyRelease(NSInputManager::Key_Tab);
			break;
		case (GLFW_KEY_LEFT_BRACKET) :
			nsengine.input()->keyRelease(NSInputManager::Key_LBracket);
			break;
		case (GLFW_KEY_RIGHT_BRACKET) :
			nsengine.input()->keyRelease(NSInputManager::Key_RBracket);
			break;
		case (GLFW_KEY_BACKSLASH) :
			nsengine.input()->keyRelease(NSInputManager::Key_BSlash);
			break;
		case (GLFW_KEY_CAPS_LOCK) :
			nsengine.input()->keyRelease(NSInputManager::Key_CapsLock);
			break;
		case (GLFW_KEY_SEMICOLON) :
			nsengine.input()->keyRelease(NSInputManager::Key_Semicolon);
			break;
		case (GLFW_KEY_APOSTROPHE) :
			nsengine.input()->keyRelease(NSInputManager::Key_Apostrophe);
			break;
		case (GLFW_KEY_ENTER) :
			nsengine.input()->keyRelease(NSInputManager::Key_Enter);
			break;
		case (GLFW_KEY_LEFT_SHIFT) :
			nsengine.input()->keyRelease(NSInputManager::Key_LShift);
			break;
		case (GLFW_KEY_COMMA) :
			nsengine.input()->keyRelease(NSInputManager::Key_Comma);
			break;
		case (GLFW_KEY_PERIOD) :
			nsengine.input()->keyRelease(NSInputManager::Key_Period);
			break;
		case (GLFW_KEY_SLASH) :
			nsengine.input()->keyRelease(NSInputManager::Key_FSlash);
			break;
		case (GLFW_KEY_RIGHT_SHIFT) :
			nsengine.input()->keyRelease(NSInputManager::Key_RShift);
			break;
		case (GLFW_KEY_LEFT_CONTROL) :
			nsengine.input()->keyRelease(NSInputManager::Key_LCtrl);
			break;
		case (GLFW_KEY_LEFT_SUPER) :
			nsengine.input()->keyRelease(NSInputManager::Key_LSuper);
			break;
		case (GLFW_KEY_LEFT_ALT) :
			nsengine.input()->keyRelease(NSInputManager::Key_LAlt);
			break;
		case (GLFW_KEY_SPACE) :
			nsengine.input()->keyRelease(NSInputManager::Key_Space);
			break;
		case (GLFW_KEY_RIGHT_ALT) :
			nsengine.input()->keyRelease(NSInputManager::Key_RAlt);
			break;
		case (GLFW_KEY_RIGHT_SUPER) :
			nsengine.input()->keyRelease(NSInputManager::Key_RSuper);
			break;
		case (GLFW_KEY_RIGHT_CONTROL) :
			nsengine.input()->keyRelease(NSInputManager::Key_RCtrl);
			break;
		case (GLFW_KEY_LEFT) :
			nsengine.input()->keyRelease(NSInputManager::Key_Left);
			break;
		case (GLFW_KEY_RIGHT) :
			nsengine.input()->keyRelease(NSInputManager::Key_Right);
			break;
		case (GLFW_KEY_UP) :
			nsengine.input()->keyRelease(NSInputManager::Key_Up);
			break;
		case (GLFW_KEY_DOWN) :
			nsengine.input()->keyRelease(NSInputManager::Key_Down);
			break;
		case (GLFW_KEY_ESCAPE) :
			nsengine.input()->keyRelease(NSInputManager::Key_Esc);
			break;
		case (GLFW_KEY_F1) :
			nsengine.input()->keyRelease(NSInputManager::Key_F1);
			break;
		case (GLFW_KEY_F2) :
			nsengine.input()->keyRelease(NSInputManager::Key_F2);
			break;
		case (GLFW_KEY_F3) :
			nsengine.input()->keyRelease(NSInputManager::Key_F3);
			break;
		case (GLFW_KEY_F4) :
			nsengine.input()->keyRelease(NSInputManager::Key_F4);
			break;
		case (GLFW_KEY_F5) :
			nsengine.input()->keyRelease(NSInputManager::Key_F5);
			break;
		case (GLFW_KEY_F6) :
			nsengine.input()->keyRelease(NSInputManager::Key_F6);
			break;
		case (GLFW_KEY_F7) :
			nsengine.input()->keyRelease(NSInputManager::Key_F7);
			break;
		case (GLFW_KEY_F8) :
			nsengine.input()->keyRelease(NSInputManager::Key_F8);
			break;
		case (GLFW_KEY_F9) :
			nsengine.input()->keyRelease(NSInputManager::Key_F9);
			break;
		case (GLFW_KEY_F10) :
			nsengine.input()->keyRelease(NSInputManager::Key_F10);
			break;
		case (GLFW_KEY_F11) :
			nsengine.input()->keyRelease(NSInputManager::Key_F11);
			break;
		case (GLFW_KEY_F12) :
			nsengine.input()->keyRelease(NSInputManager::Key_F12);
			break;
		case (GLFW_KEY_F13) :
			nsengine.input()->keyRelease(NSInputManager::Key_F13);
			break;
		case (GLFW_KEY_F14) :
			nsengine.input()->keyRelease(NSInputManager::Key_F14);
			break;
		case (GLFW_KEY_PRINT_SCREEN) :
			nsengine.input()->keyRelease(NSInputManager::Key_PrntScrn);
			break;
		case (GLFW_KEY_SCROLL_LOCK) :
			nsengine.input()->keyRelease(NSInputManager::Key_ScrollLock);
			break;
		case (GLFW_KEY_PAUSE) :
			nsengine.input()->keyRelease(NSInputManager::Key_Pause);
			break;
		case (GLFW_KEY_INSERT) :
			nsengine.input()->keyRelease(NSInputManager::Key_Insert);
			break;
		case (GLFW_KEY_DELETE) :
			nsengine.input()->keyRelease(NSInputManager::Key_Delete);
			break;
		case (GLFW_KEY_PAGE_UP) :
			nsengine.input()->keyRelease(NSInputManager::Key_PGUp);
			break;
		case (GLFW_KEY_PAGE_DOWN) :
			nsengine.input()->keyRelease(NSInputManager::Key_PGDown);
			break;
		case (GLFW_KEY_HOME) :
			nsengine.input()->keyRelease(NSInputManager::Key_Home);
			break;
		case (GLFW_KEY_END) :
			nsengine.input()->keyRelease(NSInputManager::Key_End);
			break;
		case (GLFW_KEY_NUM_LOCK) :
			nsengine.input()->keyRelease(NSInputManager::Key_NumLock);
			break;
		case (GLFW_KEY_KP_DIVIDE) :
			nsengine.input()->keyRelease(NSInputManager::Key_KPDivide);
			break;
		case (GLFW_KEY_KP_MULTIPLY) :
			nsengine.input()->keyRelease(NSInputManager::Key_KPMultiply);
			break;
		case (GLFW_KEY_KP_SUBTRACT) :
			nsengine.input()->keyRelease(NSInputManager::Key_KPSubtract);
			break;
		case (GLFW_KEY_KP_ADD) :
			nsengine.input()->keyRelease(NSInputManager::Key_KPAdd);
			break;
		case (GLFW_KEY_KP_DECIMAL) :
			nsengine.input()->keyRelease(NSInputManager::Key_KPDecimal);
			break;
		case (GLFW_KEY_KP_ENTER) :
			nsengine.input()->keyRelease(NSInputManager::Key_KPEnter);
			break;
		case (GLFW_KEY_KP_EQUAL) :
			nsengine.input()->keyRelease(NSInputManager::Key_KPEqual);
			break;
		case (GLFW_KEY_KP_0) :
			nsengine.input()->keyRelease(NSInputManager::Key_KP0);
			break;
		case (GLFW_KEY_KP_1) :
			nsengine.input()->keyRelease(NSInputManager::Key_KP1);
			break;
		case (GLFW_KEY_KP_2) :
			nsengine.input()->keyRelease(NSInputManager::Key_KP2);
			break;
		case (GLFW_KEY_KP_3) :
			nsengine.input()->keyRelease(NSInputManager::Key_KP3);
			break;
		case (GLFW_KEY_KP_4) :
			nsengine.input()->keyRelease(NSInputManager::Key_KP4);
			break;
		case (GLFW_KEY_KP_5) :
			nsengine.input()->keyRelease(NSInputManager::Key_KP5);
			break;
		case (GLFW_KEY_KP_6) :
			nsengine.input()->keyRelease(NSInputManager::Key_KP6);
			break;
		case (GLFW_KEY_KP_7) :
			nsengine.input()->keyRelease(NSInputManager::Key_KP7);
			break;
		case (GLFW_KEY_KP_8) :
			nsengine.input()->keyRelease(NSInputManager::Key_KP8);
			break;
		case (GLFW_KEY_KP_9) :
			nsengine.input()->keyRelease(NSInputManager::Key_KP9);
			break;
		default:
			nsengine.input()->keyRelease(NSInputManager::Key_None);
		}
	}
}

void GLFWMouseButtonCallback(GLFWwindow * pWindow, int pButton, int pAction, int pMods)
{
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

	if (pAction == GLFW_PRESS)
	{
		switch (pButton)
		{
		case (GLFW_MOUSE_BUTTON_LEFT) :
			nsengine.input()->mousePress(NSInputManager::LeftButton, normXPos, normYPos);
			break;
		case (GLFW_MOUSE_BUTTON_RIGHT) :
			nsengine.input()->mousePress(NSInputManager::RightButton, normXPos, normYPos);
			break;
		case (GLFW_MOUSE_BUTTON_MIDDLE) :
			nsengine.input()->mousePress(NSInputManager::MiddleButton, normXPos, normYPos);
			break;
		case (GLFW_MOUSE_BUTTON_4) :
			nsengine.input()->mousePress(NSInputManager::AuxButton1, normXPos, normYPos);
			break;
		case (GLFW_MOUSE_BUTTON_5) :
			nsengine.input()->mousePress(NSInputManager::AuxButton2, normXPos, normYPos);
			break;
		case (GLFW_MOUSE_BUTTON_6) :
			nsengine.input()->mousePress(NSInputManager::AuxButton3, normXPos, normYPos);
			break;
		case (GLFW_MOUSE_BUTTON_7) :
			nsengine.input()->mousePress(NSInputManager::AuxButton4, normXPos, normYPos);
			break;
		default:
			nsengine.input()->mousePress(NSInputManager::NoButton, normXPos, normYPos);
		}
	}
	else if (pAction == GLFW_RELEASE)
	{
		switch (pButton)
		{
		case (GLFW_MOUSE_BUTTON_LEFT) :
			nsengine.input()->mouseRelease(NSInputManager::LeftButton, normXPos, normYPos);
			break;
		case (GLFW_MOUSE_BUTTON_RIGHT) :
			nsengine.input()->mouseRelease(NSInputManager::RightButton, normXPos, normYPos);
			break;
		case (GLFW_MOUSE_BUTTON_MIDDLE) :
			nsengine.input()->mouseRelease(NSInputManager::MiddleButton, normXPos, normYPos);
			break;
		case (GLFW_MOUSE_BUTTON_4) :
			nsengine.input()->mouseRelease(NSInputManager::AuxButton1, normXPos, normYPos);
			break;
		case (GLFW_MOUSE_BUTTON_5) :
			nsengine.input()->mouseRelease(NSInputManager::AuxButton2, normXPos, normYPos);
			break;
		case (GLFW_MOUSE_BUTTON_6) :
			nsengine.input()->mouseRelease(NSInputManager::AuxButton3, normXPos, normYPos);
			break;
		case (GLFW_MOUSE_BUTTON_7) :
			nsengine.input()->mouseRelease(NSInputManager::AuxButton4, normXPos, normYPos);
			break;
		default:
			nsengine.input()->mouseRelease(NSInputManager::NoButton, normXPos, normYPos);
		}
	}
}

void GLFWCursorPosCallback(GLFWwindow * pWindow, double pPosX, double pPosY)
{
	int frameBufX = 0, frameBufY = 0, winX = 0, winY = 0;
	glfwGetFramebufferSize(pWindow, &frameBufX, &frameBufY);
	glfwGetWindowSize(pWindow, &winX, &winY);

	pPosY = winY - pPosY; // Switch to opengl coords

	// normalize coords
	pPosY /= double(frameBufY);
	pPosX /= double(frameBufX);


	nsengine.input()->mouseMove(nsfloat(pPosX), nsfloat(pPosY));
}

void GLFWScrollCallback(GLFWwindow * pWindow, double pXOffset, double pYOffset)
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

	nsengine.input()->mouseScroll(nsfloat(pYOffset), nsfloat(xPos), nsfloat(yPos));
}
#endif

#endif