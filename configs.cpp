#include <base.h>
#include <sys.h>
#include "common.h"
#include "BaseUI.h"

#include "configs.h"

config_t gConfigs;
SkillKeyConfig_s g_SkillKeyConfig;

void Config_Init(void)
{
	Sys_GetRegKeyValueUnderRoot("Software\\Valve\\Steam", "Language", gConfigs.szLanguage, 127, "");
	if (!gConfigs.szLanguage[0])
		strcpy(gConfigs.szLanguage, "english");

	/*const char *gamedir;
	CommandLine()->CheckParm("-game", &gamedir);*/
	strcpy(gConfigs.szGameName, "Counter-Strike: BreakThrough Edition 1");
	strcpy(gConfigs.szGameDir, "cstrike");
	strcpy(gConfigs.szGameVersion, "1.10");
	strcpy(gConfigs.szDownloadURL, "");
	strcpy(gConfigs.szFallbackDir, "CSBTE");
	
	if (stricmp(gConfigs.szLanguage, "english"))
		sprintf(gConfigs.szGameLangDir, "cstrike_%s", gConfigs.szLanguage);
	else
		strcpy(gConfigs.szGameLangDir, "cstrike");

	//strcpy(gConfigs.szLanguage, gConfigs.szLanguage);

	gConfigs.bEnableNewHud = true;
	gConfigs.bEnableNewMuzzle = true;
	gConfigs.bEnableFireDlight = true;
	gConfigs.bEnableBlood = true;
	gConfigs.bEnableShader = false;
	strcpy(gConfigs.szCSOLanguageName, "chn");
	strcpy(gConfigs.szCustomFontName, "DFYuanW9-GB");

	char configPath[MAX_PATH];
	
	FILE *fp;
	char line[128];
	char *data;

	sprintf(configPath, "%s/configs.ini", gConfigs.szGameLangDir);

	fp = fopen(configPath, "r");

	if (!fp)
	{
		char configPath2[MAX_PATH];
		sprintf(configPath2, "%s/configs.ini", gConfigs.szGameDir);
		CreateDirectory(gConfigs.szGameLangDir, NULL);
		CopyFile(configPath2, configPath, TRUE);

		fp = fopen(configPath2, "r");
	}

	if (fp)
	{
		while (!feof(fp))
		{
			fgets(line, sizeof(line), fp);

			if (strstr(line, "EnableNewHud") && strstr(line, "\"0\""))
				gConfigs.bEnableNewHud = false;
			else if (strstr(line, "EnableNewMuzzleflash") && strstr(line, "\"0\""))
				gConfigs.bEnableNewMuzzle = false;
			else if (strstr(line, "EnableFireDlight") && strstr(line, "\"0\""))
				gConfigs.bEnableFireDlight = false;
			else if (strstr(line, "EnableBlood") && strstr(line, "\"0\""))
				gConfigs.bEnableBlood = false;
			else if (strstr(line, "\"CSOLanguageName\""))
			{
				data = line + strlen("\"CSOLanguageName\"");
				if (strlen(data) > 0)
				{
					char *szName = &gConfigs.szCSOLanguageName[0];
					while (*data++ != '\"') {}
					while ((*szName++ = *data++) != '\"') {}
					*(szName - 1) = NULL;
				}

			}
			else if (strstr(line, "\"CustomFontName\""))
			{
				data = line + strlen("\"CustomFontName\"");
				if (strlen(data) > 0)
				{
					char *szName = &gConfigs.szCustomFontName[0];
					while (*data++ != '\"') {}
					while ((*szName++ = *data++) != '\"') {}
					*(szName - 1) = NULL;
				}
			}
		}
		
		fclose(fp);
	}

	Config_ReadSkillKey();

	gConfigs.bInitialied = true;
}

void Config_Save(void)
{
	DeleteFile(va("%s\\configs.ini", gConfigs.szGameLangDir));
	FILE *fp = fopen(va("%s\\configs.ini", gConfigs.szGameLangDir), "a");
	if (!fp)
		return;

	fprintf(fp, va("\"EnableNewHud\" \"%d\"\n", gConfigs.bEnableNewHud ? 1 : 0));
	fprintf(fp, va("\"EnableNewMuzzleflash\" \"%d\"\n", gConfigs.bEnableNewMuzzle ? 1 : 0));
	fprintf(fp, va("\"EnableFireDlight\" \"%d\"\n", gConfigs.bEnableFireDlight ? 1 : 0));
	fprintf(fp, va("\"EnableBlood\" \"%d\"\n", gConfigs.bEnableBlood ? 1 : 0));
	fprintf(fp, va("\"CSOLanguageName\" \"%s\"\n", gConfigs.szCSOLanguageName));
	fprintf(fp, va("\"CustomFontName\" \"%s\"", gConfigs.szCustomFontName));

	fclose(fp);

	return;
}

void Config_ReadSkillKey(void)
{
	
	int iZombieSkillKeyDefault[4] = { 6, 1, 2, 3 };
	int iHumanSkillKeyDefault[4] = { 1, 2, 3, 4 };
	
	char szPath[256];
	sprintf(szPath, "%s\\SkillKey.ini", gConfigs.szGameLangDir);

	// 判断下技能栏有没有重复（防止乱改ini）
	int bitsHumanSkillSlotChecked = 0;

	for (int i = 0; i < 4; i++)
	{
		g_SkillKeyConfig.iHumanSlotAlias[i] = clamp((int)GetPrivateProfileInt("HumanSkill", va("SkillSlot%d", i + 1), i, szPath), 0, 3);
		bitsHumanSkillSlotChecked |= (1 << g_SkillKeyConfig.iHumanSlotAlias[i]);

		g_SkillKeyConfig.iHumanSkillKey[i] = clamp((int)GetPrivateProfileInt("HumanSkill", va("SkillKey%d", i + 1), iHumanSkillKeyDefault[i], szPath), 0, 9);

		g_SkillKeyConfig.iZombieSkillKey[i] = clamp((int)GetPrivateProfileInt("ZombieSkill", va("SkillKey%d", i + 1), iZombieSkillKeyDefault[i], szPath), 0, 10);
	}

	// 如果重复掉重新填上0123
	if (bitsHumanSkillSlotChecked != (1 << 4) - 1)
	{
		for (int i = 0; i < 4; i++)
			g_SkillKeyConfig.iHumanSlotAlias[i] = i;
	}
}

void Config_SaveSkillKey()
{
	char szPath[256];
	sprintf(szPath, "%s\\SkillKey.ini", gConfigs.szGameLangDir);

	char szData[4];
	for (int i = 0; i < 4; i++)
	{
		sprintf(szData, "%d", g_SkillKeyConfig.iHumanSlotAlias[i]);
		WritePrivateProfileStringA("HumanSkill", va("SkillSlot%d", i + 1), szData, szPath);
		sprintf(szData, "%d", g_SkillKeyConfig.iHumanSkillKey[i]);
		WritePrivateProfileStringA("HumanSkill", va("SkillKey%d", i + 1), szData, szPath);
		sprintf(szData, "%d", g_SkillKeyConfig.iZombieSkillKey[i]);
		WritePrivateProfileStringA("ZombieSkill", va("SkillKey%d", i + 1), szData, szPath);
	}
	

	return;
}
/*
void Config_LoadLocalize()
{
	vgui::localize()->RemoveAll();

	// Other
	vgui::localize()->AddFile(g_pFileSystem, va("Resource/cso_%s.txt", gConfigs.szCSOLanguageName));
	vgui::localize()->AddFile(g_pFileSystem, "Resource/csbte_%language%.txt");
	vgui::localize()->AddFile(g_pFileSystem, "Resource/mapname_%language%.txt");
	vgui::localize()->AddFile(g_pFileSystem, va("Resource/cstrike_%s.txt", gConfigs.szCSOLanguageName));
	
	// GameUI
	vgui::localize()->AddFile(g_pFileSystem, "Resource/gameui_%language%.txt");
	vgui::localize()->AddFile(g_pFileSystem, "Resource/valve_%language%.txt");
	vgui::localize()->AddFile(g_pFileSystem, "Resource/vgui_%language%.txt");

	// ServerBrowser
	vgui::localize()->AddFile(g_pFileSystem, "servers/serverbrowser_%language%.txt");
}
*/