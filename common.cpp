#include <windows.h>
#include "tier1/characterset.h"
#include "tier1/strtools.h"
#include "common.h"

#include <stdio.h>
#include "sys.h"

#define COM_TOKEN_MAX_LENGTH 1024

char com_token[COM_TOKEN_MAX_LENGTH];
bool com_ignorecolons = false;

static characterset_t g_BreakSet, g_BreakSetIncludingColons;

void COM_Init(void)
{
	CharacterSetBuild(&g_BreakSet, "{}()'");
	CharacterSetBuild(&g_BreakSetIncludingColons, "{}()':");
}

const char *COM_Parse(const char *data)
{
	unsigned char c;
	int len;
	characterset_t *breaks;

	breaks = &g_BreakSetIncludingColons;

	if (com_ignorecolons)
		breaks = &g_BreakSet;

	len = 0;
	com_token[0] = 0;

	if (!data)
		return NULL;

skipwhite:
	while ((c = *data) <= ' ')
	{
		if (c == 0)
			return NULL;

		data++;
	}

	if (c == '/' && data[1] == '/')
	{
		while (*data && *data != '\n')
			data++;

		goto skipwhite;
	}

	if (c == '\"')
	{
		data++;

		while (1)
		{
			c = *data++;

			if (c == '\"' || !c)
			{
				com_token[len] = 0;
				return data;
			}

			com_token[len] = c;
			len++;
		}
	}

	if (IN_CHARACTERSET(*breaks, c))
	{
		com_token[len] = c;
		len++;
		com_token[len] = 0;
		return data + 1;
	}
	do
	{
		com_token[len] = c;
		data++;
		len++;
		c = *data;

		if (IN_CHARACTERSET(*breaks, c))
			break;
	}
	while (c > 32);

	com_token[len] = 0;
	return data;
}

const char *COM_ParseFile(const char *data, char *token, int maxtoken)
{
	const char *return_data = COM_Parse(data);
	Q_strncpy(token, com_token, maxtoken);
	return return_data;
}

void COM_CreatePath(const char *path)
{
	char temppath[512];
	Q_strncpy(temppath, path, sizeof(temppath));

	for (char *ofs = temppath + 1; *ofs; ofs++)
	{
		if (*ofs == '/' || *ofs == '\\')
		{
			char old = *ofs;
			*ofs = 0;
			Sys_mkdir(temppath);
			*ofs = old;
		}
	}
}

char *COM_GetToken(void)
{
	return com_token;
}

char *va(char *format, ...)
{
	va_list argptr;
	static char string[4][1024];
	static int curstring = 0;

	curstring = (curstring + 1) % 4;

	va_start(argptr, format);
	Q_vsnprintf(string[curstring], sizeof(string[curstring]), format, argptr);
	va_end(argptr);

	return string[curstring];
}