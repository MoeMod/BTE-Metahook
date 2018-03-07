#pragma once

#ifndef MAX_PATH
#define MAX_PATH 128
#endif

typedef struct SkillKeyConfig_s
{
	int iHumanSlotAlias[4];
	int iHumanSkillKey[4];
	int iZombieSkillKey[4];

}SkillKeyConfig_t;

typedef struct config_s
{
	bool bInitialied;

	char szLanguage[128];
	char szGameDir[MAX_PATH];
	char szGameLangDir[MAX_PATH];
	char szFallbackDir[MAX_PATH];
	char szGameVersion[12];
	char szDownloadURL[256];
	char szGameName[128];
	char szCustomFontName[128];
	char szCSOLanguageName[128];

	bool bEnableNewHud;
	bool bEnableNewMuzzle;
	bool bEnableFireDlight;
	bool bEnableBlood;
	bool bEnableShader;
}
config_t;

extern config_t gConfigs;

void Config_Init(void);
void Config_Save(void);
void Config_ReadSkillKey(void);
void Config_SaveSkillKey(void);

//void Config_LoadLocalize();

extern SkillKeyConfig_s g_SkillKeyConfig;