#pragma once
#include "gl/gl.h"

GLuint GL_LoadTexture(const char *szPath, int *pwidth = nullptr, int *pheight = nullptr);
void GL_DrawTexture(GLuint index, int r, int g, int b, int a, int iX, int iY, int iWidth, int iHeight);