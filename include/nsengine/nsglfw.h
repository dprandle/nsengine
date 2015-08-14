#ifndef NSGLFW_H
#define NSGLFW_H

#include <nsmath.h>
#include <GLFW/glfw3.h>

bool glfw_setup(const ivec2 & screendim, nsbool fullscreen, const nsstring & title);

bool glfw_window_open();

void glfw_update();

void glfw_keypress_callback(GLFWwindow * pWindow, int pKey, int pScancode, int pAction, int pMods);

void glfw_mousebutton_callback(GLFWwindow * pWindow, int pButton, int pAction, int pMods);

void glfw_cursorpos_callback(GLFWwindow * pWindow, double pPosX, double pPosY);

void glfw_scroll_callback(GLFWwindow * pWindow, double pXOffset, double pYOffset);

void glfw_resizewindow_callback(GLFWwindow* window, int width, int height);

#endif
