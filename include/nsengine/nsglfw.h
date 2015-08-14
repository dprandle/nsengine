#ifndef NSGLFW_H
#define NSGLFW_H

#include <nsmath.h>
#include <GLFW/glfw3.h>

bool glfw_setup(const ivec2 & screendim, bool fullscreen, const nsstring & title);

bool glfw_window_open();

void glfw_update();

void glfw_keypress_callback(GLFWwindow * pWindow, int32 pKey, int32 pScancode, int32 pAction, int32 pMods);

void glfw_mousebutton_callback(GLFWwindow * pWindow, int32 pButton, int32 pAction, int32 pMods);

void glfw_cursorpos_callback(GLFWwindow * pWindow, double pPosX, double pPosY);

void glfw_scroll_callback(GLFWwindow * pWindow, double pXOffset, double pYOffset);

void glfw_resizewindow_callback(GLFWwindow* window, int32 width, int32 height);

#endif
