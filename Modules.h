extern HMODULE g_hThisModule;
extern HMODULE g_hClientModule;

void Module_InstallHook(void);
void Module_LoadClient(cl_exportfuncs_t *pExportFunc);
void Module_UninstallHook(void);
void Module_Shutdown(void);