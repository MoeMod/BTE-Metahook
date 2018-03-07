char *va(char *format, ...);

void COM_Init(void);
const char *COM_Parse(const char *data);
const char *COM_ParseFile(const char *data, char *token, int maxtoken);
char *COM_GetToken(void);
void COM_CreatePath(const char *path);