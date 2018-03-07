#pragma once

bool LoadDDS(const char *szFilename, byte *buffer, int bufferSize, int *width, int *height, GLuint *compressFormat);
int GetDDSSize(const char *szFilename, int *w, int *h);