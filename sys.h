void Sys_GetRegKeyValueUnderRoot(const char *pszSubKey, const char *pszElement, char *pszReturnString, int nReturnLength, const char *pszDefaultValue);
void Sys_SetRegKeyValueUnderRoot(const char *pszSubKey, const char *pszElement, const char *pszValue);
void Sys_mkdir(const char *path);
void Sys_ErrorEx(const char *error);