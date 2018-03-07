#pragma once
bool LoadDTX(const char *szFilename, byte *buffer, int bufferSize, int *width, int *height, int*pInternalFormat, int *pImageSize);
bool GetDTXSize(const char *pszFileName, int *width, int *height);