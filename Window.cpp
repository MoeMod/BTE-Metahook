#include "metahook.h"
#include "plugins.h"
#include "Screen.h"
#include "Window.h"

HWND g_hWnd = NULL;

namespace DPIManger
{
	typedef enum PROCESS_DPI_AWARENESS {
		PROCESS_DPI_UNAWARE = 0,
		PROCESS_SYSTEM_DPI_AWARE = 1,
		PROCESS_PER_MONITOR_DPI_AWARE = 2
	} PROCESS_DPI_AWARENESS;
	// Win8.1 supports monitor-specific DPI scaling.
	static bool SetProcessDpiAwareness(PROCESS_DPI_AWARENESS value) {
		typedef BOOL(WINAPI *SetProcessDpiAwarenessPtr)(PROCESS_DPI_AWARENESS);
		SetProcessDpiAwarenessPtr set_process_dpi_awareness_func =
			reinterpret_cast<SetProcessDpiAwarenessPtr>(
			GetProcAddress(GetModuleHandleA("user32.dll"),
			"SetProcessDpiAwarenessInternal"));
		if (set_process_dpi_awareness_func) {
			HRESULT hr = set_process_dpi_awareness_func(value);
			if (SUCCEEDED(hr)) {
				//VLOG(1) << "SetProcessDpiAwareness succeeded.";
				return true;
			}
			else if (hr == E_ACCESSDENIED) {
				//LOG(ERROR) << "Access denied error from SetProcessDpiAwareness. "
				//	"Function called twice, or manifest was used.";
			}
		}
		return false;
	}
	static void Init()
	{
		SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
	}
};

BOOL CALLBACK Window_EnumWindowsProc(HWND hWnd, LPARAM);

void Window_Init(void)
{
	DPIManger::Init();
}

void Window_LoadClient(void)
{
	EnumThreadWindows(GetCurrentThreadId(), Window_EnumWindowsProc, 0);
}

void Window_Reset(void)
{
	Window_SetTitle("Counter-Strike: BreakThrough Edition - Loading...");
}

void Window_VidInit(void)
{
	char title[255];
	sprintf(title, "%s [%s]", "Counter-Strike: BreakThrough Edition", __DATE__);
	Window_SetTitle(title);

	if (gEngfuncs.CheckParm("-noborder", NULL))
	{
		int width = ScreenInfo().iWidth;
		int height = ScreenInfo().iHeight;
		int x = (GetSystemMetrics(SM_CXSCREEN) - width) / 2;
		int y = (GetSystemMetrics(SM_CYSCREEN) - height) / 2;

		SetWindowLong(g_hWnd, GWL_STYLE, WS_CLIPSIBLINGS | WS_POPUP | WS_VISIBLE);
		MoveWindow(g_hWnd, x, y, width, height, FALSE);
	}
}

void Window_SetTitle(char *title)
{
	SetWindowText(g_hWnd, title);
}

BOOL CALLBACK Window_EnumWindowsProc(HWND hWnd, LPARAM)
{
	char szClassName[32];
	GetClassName(hWnd, szClassName, sizeof(szClassName));

	if (!strcmp(szClassName, "Valve001") || !strcmp(szClassName, "SDL_app"))
	{
		g_hWnd = hWnd;
		Window_Reset();
		return FALSE;
	}

	return TRUE;
}