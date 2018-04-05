
int LoadTGA(const char *name, byte *targa_rgba, int bufferSize, int *width, int *height, int FlipTga = 0);
bool GetTGASize(const char *szFilename, int *width, int *height);
bool WriteTGA(byte *pixels, int width, int height, const char *szFilename);