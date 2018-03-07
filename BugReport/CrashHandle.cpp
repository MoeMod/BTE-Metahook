#define _WINSOCKAPI_
#include <metahook.h>
#include <assert.h>
#include <Dbghelp.h>
//#define _WIN32_WINNT 0x500
#include <Winternl.h>
#include <stdio.h>
#include "CSmtp.h"
#include "Encode.h"
#include "XorStr.h"
#include "BugReport.h"
#include "configs.h"

#define USE_HOOKMDMP

#pragma comment(lib, "DbgHelp.lib")

#ifdef _DEBUG

bool symbolsinit = false;

void InitSymbols(void)
{
	if (!symbolsinit)
	{
		DWORD dwOptions = SymGetOptions();

		SymSetOptions(dwOptions | SYMOPT_LOAD_LINES);

		BOOL blah = SymInitialize(GetCurrentProcess(), NULL, TRUE);
		assert(blah);

		symbolsinit = true;
	}
}

void KillSymbols(void)
{
	if (symbolsinit)
	{
		SymCleanup(GetCurrentProcess());
		symbolsinit = false;
	}
}

#endif

LONG WINAPI GetCrashReason(LPEXCEPTION_POINTERS ep)
{
	if (!ep)
		return EXCEPTION_CONTINUE_EXECUTION;

	if (IsBadReadPtr(ep, sizeof(EXCEPTION_POINTERS)))
		return EXCEPTION_CONTINUE_EXECUTION;

#ifdef _DEBUG
	InitSymbols();

	DWORD dwLineDisp = 0;
	IMAGEHLP_LINE64 crashline = { sizeof(IMAGEHLP_LINE64), NULL, 0, NULL, 0 };

	if (SymGetLineFromAddr64(GetCurrentProcess(), (DWORD64)ep->ExceptionRecord->ExceptionAddress, &dwLineDisp, &crashline))
	{
		char msg[2048];
		sprintf(msg, "file: %s  line: %i", crashline.FileName, (int)crashline.LineNumber);
		MessageBox(NULL, msg, "An error has occurred", MB_OK | MB_ICONSTOP);
	}
	else
		MessageBox(NULL, "Unknown error", "An error has occurred", MB_OK | MB_ICONSTOP);

	KillSymbols();
#endif
#if 0
	Gamma_Restore();
#endif
	return EXCEPTION_EXECUTE_HANDLER;
}

//#define NT_SUCCESS(Status) ((NTSTATUS)(Status) >= 0)

typedef struct _OBJECT_NAME_INFORMATION
{
	UNICODE_STRING Name;
}
OBJECT_NAME_INFORMATION, *POBJECT_NAME_INFORMATION;

/*typedef enum _OBJECT_INFORMATION_CLASS
{
ObjectBasicInformation = 0,
ObjectNameInformation = 1,
ObjectTypeInformation = 2
}
OBJECT_INFORMATION_CLASS;*/

BOOL(WINAPI *g_pfnMiniDumpWriteDump)(HANDLE hProcess, DWORD ProcessId, HANDLE hFile, MINIDUMP_TYPE DumpType, CONST PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam, CONST PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam, CONST PMINIDUMP_CALLBACK_INFORMATION CallbackParam);
NTSTATUS(NTAPI *NtQueryObject0)(HANDLE Handle, OBJECT_INFORMATION_CLASS ObjectInformationClass, PVOID ObjectInformation, ULONG ObjectInformationLength, PULONG ReturnLength);

#if defined(_WIN32) || defined(WIN32)
#define PATHSEPARATOR(c) ((c) == '\\' || (c) == '/')
#else
#define PATHSEPARATOR(c) ((c) == '/')
#endif

void V_FileBase(const char *in, char *out, int maxlen)
{
	assert(maxlen >= 1);
	assert(in);
	assert(out);

	if (!in || !in[0])
	{
		*out = 0;
		return;
	}

	int len, start, end;

	len = strlen(in);
	end = len - 1;

	while (end && in[end] != '.' && !PATHSEPARATOR(in[end]))
		end--;

	if (in[end] != '.')
		end = len - 1;
	else
		end--;

	start = len - 1;

	while (start >= 0 && !PATHSEPARATOR(in[start]))
		start--;

	if (start < 0 || !PATHSEPARATOR(in[start]))
		start = 0;
	else
		start++;

	len = end - start + 1;

	int maxcopy = min(len + 1, maxlen);

	strncpy(out, &in[start], maxcopy);
	out[maxcopy - 1] = 0;
}

bool g_bSendBugReport = false;
bool g_bRestartGame = false;
bool g_bDeleteThisMDMP = false;
char g_szBugReportDesc[1024];

BOOL WINAPI Hook_MiniDumpWriteDump(HANDLE hProcess, DWORD ProcessId, HANDLE hFile, MINIDUMP_TYPE DumpType, CONST PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam, CONST PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam, CONST PMINIDUMP_CALLBACK_INFORMATION CallbackParam)
{
	BOOL result;

	if (DumpType != MiniDumpNormal)
		return TRUE;

#if 0
	Gamma_Restore();
	ExtDLL_Release();
#endif
	if (g_pInterface->CommandLine->CheckParm("-nomdmp"))
		return FALSE;
#if 0
	if (!VideoMode_IsWindowed())
		VID_SwitchFullScreen(false);

	VID_HideWindow();
	VID_CloseWindow();
#endif
	if (BugReport_CreateWindow())
		BugReport_MainLoop();

	result = g_pfnMiniDumpWriteDump(hProcess, ProcessId, hFile, DumpType, ExceptionParam, UserStreamParam, CallbackParam);

	if (result && (g_bSendBugReport || g_bDeleteThisMDMP))
	{
		POBJECT_NAME_INFORMATION pa = NULL;
		char cInfoBuffer[0x10000];
		ULONG ulSize;
		PWSTR ObjectName;

		if (NT_SUCCESS(NtQueryObject0(hFile, (OBJECT_INFORMATION_CLASS)1, cInfoBuffer, sizeof(cInfoBuffer), &ulSize)))
		{
			pa = (POBJECT_NAME_INFORMATION)cInfoBuffer;
			ObjectName = pa->Name.Buffer;

			char filebase[MAX_PATH];
			char *filename = UnicodeToANSI(ObjectName);
			char filepath[MAX_PATH], direcotry[MAX_PATH];
			DWORD dwFileSize;
			V_FileBase(filename, filebase, sizeof(filebase));
			GetCurrentDirectory(sizeof(direcotry), direcotry);
			sprintf(filepath, "%s\\%s.mdmp", direcotry, filebase);
			dwFileSize = GetFileSize(hFile, NULL);
			CloseHandle(hFile);

			//Removed Sent Email Feature//
			//Sent By Their Self//

			if (g_bDeleteThisMDMP)
			{
				DeleteFile(filepath);
			}
		}
	}

	if (g_bRestartGame)
	{
		HANDLE hObject = CreateMutex(NULL, FALSE, "ValveHalfLifeLauncherMutex");

		if (hObject)
		{
			ReleaseMutex(hObject);
			CloseHandle(hObject);
		}

		STARTUPINFO SI;
		PROCESS_INFORMATION PI;
		memset(&SI, 0, sizeof(SI));
		memset(&PI, 0, sizeof(PI));
		SI.cb = sizeof(STARTUPINFO);

		CreateProcess(NULL, (LPSTR)g_pInterface->CommandLine->GetCmdLine(), NULL, NULL, FALSE, CREATE_NEW_PROCESS_GROUP | NORMAL_PRIORITY_CLASS, NULL, NULL, &SI, &PI);
		TerminateProcess(GetCurrentProcess(), 1);

		gEngfuncs.pfnClientCmd("_restart");
	}

	return result;
}

void CMD_Crash(void)
{
	void(*pfnCrash)(void) = NULL;
	pfnCrash();

	*(int *)0 = 0;
}

void InitCrashHandle(void)
{
	DWORD dwMiniDumpWriteDump = (DWORD)MiniDumpWriteDump;

	if (*(BYTE *)dwMiniDumpWriteDump == 0xE9)
	{
		dwMiniDumpWriteDump += 1;
		dwMiniDumpWriteDump = dwMiniDumpWriteDump + *(DWORD *)dwMiniDumpWriteDump + 0x4;
	}

#ifdef USE_HOOKMDMP
#ifndef _DEBUG
	g_pMetaHookAPI->InlineHook((void *)dwMiniDumpWriteDump, Hook_MiniDumpWriteDump, (void *&)g_pfnMiniDumpWriteDump);
	NtQueryObject0 = (NTSTATUS(NTAPI *)(HANDLE, OBJECT_INFORMATION_CLASS, PVOID, ULONG, PULONG))GetProcAddress(GetModuleHandle("ntdll"), "NtQueryObject");
#endif
#else
	SetUnhandledExceptionFilter(GetCrashReason);
#endif
	//gEngfuncs.pfnAddCommand("crash", CMD_Crash);
}