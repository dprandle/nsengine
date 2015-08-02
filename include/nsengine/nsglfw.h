#ifndef NSGLFW_H
#define NSGLFW_H

struct GLFWwindow;
void GLFWKeyCallback(GLFWwindow * pWindow, int pKey, int pScancode, int pAction, int pMods);
void GLFWMouseButtonCallback(GLFWwindow * pWindow, int pButton, int pAction, int pMods);
void GLFWCursorPosCallback(GLFWwindow * pWindow, double pPosX, double pPosY);
void GLFWScrollCallback(GLFWwindow * pWindow, double pXOffset, double pYOffset);

#endif
