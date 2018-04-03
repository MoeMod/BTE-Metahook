
#include <metahook.h>
#include "Viewport.h"

#include "CounterStrikeViewport.h"

extern void *gpViewPortInterface;

VGuiLibraryInterface_t *(__fastcall *g_pfnGetClientDllInterface)(void *pthis, int);
void (__fastcall *g_pfnSetClientDllInterface)(void *pthis, int, VGuiLibraryInterface_t *clientInterface);
void (__fastcall *g_pfnUpdateScoreBoard)(void *pthis, int);
bool (__fastcall *g_pfnAllowedToPrintText)(void *pthis, int);
void (__fastcall *g_pfnGetAllPlayersInfo)(void *pthis, int);
void (__fastcall *g_pfnDeathMsg)(void *pthis, int, int killer, int victim);
void (__fastcall *g_pfnShowScoreBoard)(void *pthis, int);
bool (__fastcall *g_pfnCanShowScoreBoard)(void *pthis, int);
void (__fastcall *g_pfnHideAllVGUIMenu)(void *pthis, int);
void (__fastcall *g_pfnUpdateSpectatorPanel)(void *pthis, int);
bool (__fastcall *g_pfnIsScoreBoardVisible)(void *pthis, int);
void (__fastcall *g_pfnHideScoreBoard)(void *pthis, int);
int (__fastcall *g_pfnKeyInput)(void *pthis, int, int down, int keynum, const char *pszCurrentBinding);
void (__fastcall *g_pfnShowVGUIMenu)(void *pthis, int, int iMenu);
void (__fastcall *g_pfnHideVGUIMenu)(void *pthis, int, int iMenu);
void (__fastcall *g_pfnShowTutorTextWindow)(void *pthis, int, const wchar_t *szString, int id, int msgClass, int isSpectator);
void (__fastcall *g_pfnShowTutorLine)(void *pthis, int, int entindex, int id);
void (__fastcall *g_pfnShowTutorState)(void *pthis, int, const wchar_t *szString);
void (__fastcall *g_pfnCloseTutorTextWindow)(void *pthis, int);
bool (__fastcall *g_pfnIsTutorTextWindowOpen)(void *pthis, int);
void (__fastcall *g_pfnShowSpectatorGUI)(void *pthis, int);
void (__fastcall *g_pfnShowSpectatorGUIBar)(void *pthis, int);
void (__fastcall *g_pfnHideSpectatorGUI)(void *pthis, int);
void (__fastcall *g_pfnDeactivateSpectatorGUI)(void *pthis, int);
bool (__fastcall *g_pfnIsSpectatorGUIVisible)(void *pthis, int);
bool (__fastcall *g_pfnIsSpectatorBarVisible)(void *pthis, int);
int (__fastcall *g_pfnMsgFunc_ResetFade)(void *pthis, int, const char *pszName, int iSize, void *pbuf);
void (__fastcall *g_pfnSetSpectatorBanner)(void *pthis, int, const char *image);
void (__fastcall *g_pfnSpectatorGUIEnableInsetView)(void *pthis, int, int value);
void (__fastcall *g_pfnShowCommandMenu)(void *pthis, int);
void (__fastcall *g_pfnUpdateCommandMenu)(void *pthis, int);
void (__fastcall *g_pfnHideCommandMenu)(void *pthis, int);
int (__fastcall *g_pfnIsCommandMenuVisible)(void *pthis, int);
int (__fastcall *g_pfnGetValidClasses)(void *pthis, int, int iTeam);
int (__fastcall *g_pfnGetNumberOfTeams)(void *pthis, int, int iTeam);
bool (__fastcall *g_pfnGetIsFeigning)(void *pthis, int);
int (__fastcall *g_pfnGetIsSettingDetpack)(void *pthis, int);
int (__fastcall *g_pfnGetBuildState)(void *pthis, int);
int (__fastcall *g_pfnIsRandom)(void *pthis, int);
char *(__fastcall *g_pfnGetTeamName)(void *pthis, int, int iTeam);
int (__fastcall *g_pfnGetCurrentMenu)(void *pthis, int);
const char *(__fastcall *g_pfnGetMapName)(void *pthis, int);
const char *(__fastcall *g_pfnGetServerName)(void *pthis, int);
void (__fastcall *g_pfnInputPlayerSpecial)(void *pthis, int);
void (__fastcall *g_pfnOnTick)(void *pthis, int);
int (__fastcall *g_pfnGetViewPortScheme)(void *pthis, int);
vgui::VPANEL (__fastcall *g_pfnGetViewPortPanel)(void *pthis, int);
int (__fastcall *g_pfnGetAllowSpectators)(void *pthis, int);
void (__fastcall *g_pfnOnLevelChange)(void *pthis, int);
void (__fastcall *g_pfnHideBackGround)(void *pthis, int);
void (__fastcall *g_pfnChatInputPosition)(void *pthis, int, int *x, int *y);
int (__fastcall *g_pfnGetSpectatorBottomBarHeight)(void *pthis, int);
int (__fastcall *g_pfnGetSpectatorTopBarHeight)(void *pthis, int);
bool(__fastcall *g_pfnSlotInput)(void *pthis, int, int iSlot);
VGuiLibraryTeamInfo_t (__fastcall *g_pfnGetPlayerTeamInfo)(void *pthis, int, int playerIndex);
void (__fastcall *g_pfnMakeSafeName)(void *pthis, int, const char *oldName, char *newName, int newNameBufSize);

void (__fastcall *g_pfnInitialize)(void *pthis, int, CreateInterfaceFn *factories, int count);
void (__fastcall *g_pfnStart)(void *pthis, int);
void (__fastcall *g_pfnShutdown)(void *pthis, int);
void (__fastcall *g_pfnSetParent)(void *pthis, int, vgui::VPANEL parent);
bool (__fastcall *g_pfnUseVGUI1)(void *pthis, int);
void (__fastcall *g_pfnActivateClientUI)(void *pthis, int);
void (__fastcall *g_pfnHideClientUI)(void *pthis, int);
void (__fastcall *g_pfnSetNumberOfTeams)(void *pthis, int, int num);
int (__fastcall *g_pfnMsgFunc_ValClass)(void *pthis, int, const char *pszName, int iSize, void *pbuf);
int (__fastcall *g_pfnMsgFunc_TeamNames)(void *pthis, int, const char *pszName, int iSize, void *pbuf);
int (__fastcall *g_pfnMsgFunc_Feign)(void *pthis, int, const char *pszName, int iSize, void *pbuf);
int (__fastcall *g_pfnMsgFunc_Detpack)(void *pthis, int, const char *pszName, int iSize, void *pbuf);
int (__fastcall *g_pfnMsgFunc_VGUIMenu)(void *pthis, int, const char *pszName, int iSize, void *pbuf);
int (__fastcall *g_pfnMsgFunc_TutorText)(void *pthis, int, const char *pszName, int iSize, void *pbuf);
int (__fastcall *g_pfnMsgFunc_TutorLine)(void *pthis, int, const char *pszName, int iSize, void *pbuf);
int (__fastcall *g_pfnMsgFunc_TutorState)(void *pthis, int, const char *pszName, int iSize, void *pbuf);
int (__fastcall *g_pfnMsgFunc_TutorClose)(void *pthis, int, const char *pszName, int iSize, void *pbuf);
int (__fastcall *g_pfnMsgFunc_MOTD)(void *pthis, int, const char *pszName, int iSize, void *pbuf);
int (__fastcall *g_pfnMsgFunc_BuildSt)(void *pthis, int, const char *pszName, int iSize, void *pbuf);
int (__fastcall *g_pfnMsgFunc_RandomPC)(void *pthis, int, const char *pszName, int iSize, void *pbuf);
int (__fastcall *g_pfnMsgFunc_ServerName)(void *pthis, int, const char *pszName, int iSize, void *pbuf);
int (__fastcall *g_pfnMsgFunc_ScoreInfo)(void *pthis, int, const char *pszName, int iSize, void *pbuf);
int (__fastcall *g_pfnMsgFunc_TeamScore)(void *pthis, int, const char *pszName, int iSize, void *pbuf);
int (__fastcall *g_pfnMsgFunc_TeamInfo)(void *pthis, int, const char *pszName, int iSize, void *pbuf);
int (__fastcall *g_pfnMsgFunc_Spectator)(void *pthis, int, const char *pszName, int iSize, void *pbuf);
int (__fastcall *g_pfnMsgFunc_AllowSpec)(void *pthis, int, const char *pszName, int iSize, void *pbuf);
int (__fastcall *g_pfnMsgFunc_SpecFade)(void *pthis, int, const char *pszName, int iSize, void *pbuf);
bool (__fastcall *g_pfnIsVGUIMenuActive)(void *pthis, int, int iMenu);
bool (__fastcall *g_pfnIsAnyVGUIMenuActive)(void *pthis, int);
void (__fastcall *g_pfnDisplayVGUIMenu)(void *pthis, int, int iMenu);
int (__fastcall *g_pfnGetForceCamera)(void *pthis, int);
int (__fastcall *g_pfnMsgFunc_ForceCam)(void *pthis, int, const char *pszName, int iSize, void *pbuf);
int (__fastcall *g_pfnMsgFunc_Location)(void *pthis, int, const char *pszName, int iSize, void *pbuf);
void (__fastcall *g_pfnUpdateBuyPresets)(void *pthis, int);
void (__fastcall *g_pfnUpdateScheme)(void *pthis, int);
bool (__fastcall *g_pfnIsProgressBarVisible)(void *pthis, int);
void (__fastcall *g_pfnStartProgressBar)(void *pthis, int, const char *title, int numTicks, int startTicks, bool isTimeBased);
void (__fastcall *g_pfnUpdateProgressBar)(void *pthis, int, const char *statusText, int tick);
void (__fastcall *g_pfnStopProgressBar)(void *pthis, int);

class CCounterStrikeViewport : public CounterStrikeViewport
{
public:
	virtual VGuiLibraryInterface_t *GetClientDllInterface(void);
	virtual void SetClientDllInterface(VGuiLibraryInterface_t *clientInterface);
	virtual void UpdateScoreBoard(void);
	virtual bool AllowedToPrintText(void);
	virtual void GetAllPlayersInfo(void);
	virtual void DeathMsg(int killer, int victim);
	virtual void ShowScoreBoard(void);
	virtual bool CanShowScoreBoard(void);
	virtual void HideAllVGUIMenu(void);
	virtual void UpdateSpectatorPanel(void);
	virtual bool IsScoreBoardVisible(void);
	virtual void HideScoreBoard(void);
	virtual int KeyInput(int down, int keynum, const char *pszCurrentBinding);
	virtual void ShowVGUIMenu(int iMenu);
	virtual void HideVGUIMenu(int iMenu);
	virtual void ShowTutorTextWindow(const wchar_t *szString, int id, int msgClass, int isSpectator);
	virtual void ShowTutorLine(int entindex, int id);
	virtual void ShowTutorState(const wchar_t *szString);
	virtual void CloseTutorTextWindow(void);
	virtual bool IsTutorTextWindowOpen(void);
	virtual void ShowSpectatorGUI(void);
	virtual void ShowSpectatorGUIBar(void);
	virtual void HideSpectatorGUI(void);
	virtual void DeactivateSpectatorGUI(void);
	virtual bool IsSpectatorGUIVisible(void);
	virtual bool IsSpectatorBarVisible(void);
	virtual int MsgFunc_ResetFade(const char *pszName, int iSize, void *pbuf);
	virtual void SetSpectatorBanner(const char *image);
	virtual void SpectatorGUIEnableInsetView(int value);
	virtual void ShowCommandMenu(void);
	virtual void UpdateCommandMenu(void);
	virtual void HideCommandMenu(void);
	virtual int IsCommandMenuVisible(void);
	virtual int GetValidClasses(int iTeam);
	virtual int GetNumberOfTeams(int iTeam);
	virtual bool GetIsFeigning(void);
	virtual int GetIsSettingDetpack(void);
	virtual int GetBuildState(void);
	virtual int IsRandom(void);
	virtual char *GetTeamName(int iTeam);
	virtual int GetCurrentMenu(void);
	virtual const char *GetMapName(void);
	virtual const char *GetServerName(void);
	virtual void InputPlayerSpecial(void);
	virtual void OnTick(void);
	virtual int GetViewPortScheme(void);
	virtual vgui::VPANEL GetViewPortPanel(void);
	virtual int GetAllowSpectators(void);
	virtual void OnLevelChange(void);
	virtual void HideBackGround(void);
	virtual void ChatInputPosition(int *x, int *y);
	virtual int GetSpectatorBottomBarHeight(void);
	virtual int GetSpectatorTopBarHeight(void);
	virtual bool SlotInput(int iSlot);
	virtual VGuiLibraryTeamInfo_t GetPlayerTeamInfo(int playerIndex);
	virtual void MakeSafeName(const char *oldName, char *newName, int newNameBufSize);
};

CCounterStrikeViewport g_CounterStrikeViewport;
CounterStrikeViewport *g_pCounterStrikeViewpot;

VGuiLibraryInterface_t *CCounterStrikeViewport::GetClientDllInterface(void)
{
	return g_pfnGetClientDllInterface(this, 0);
}

void CCounterStrikeViewport::SetClientDllInterface(VGuiLibraryInterface_t *clientInterface)
{
	g_pfnSetClientDllInterface(this, 0, clientInterface);
}

void CCounterStrikeViewport::UpdateScoreBoard(void)
{
	g_pfnUpdateScoreBoard(this, 0);
}

bool CCounterStrikeViewport::AllowedToPrintText(void)
{
	return g_pfnAllowedToPrintText(this, 0);
}

void CCounterStrikeViewport::GetAllPlayersInfo(void)
{
	return g_pfnGetAllPlayersInfo(this, 0);
}

void CCounterStrikeViewport::DeathMsg(int killer, int victim)
{
	g_pfnDeathMsg(this, 0, killer, victim);
}

void CCounterStrikeViewport::ShowScoreBoard(void)
{
	g_pfnShowScoreBoard(this, 0);
}

bool CCounterStrikeViewport::CanShowScoreBoard(void)
{
	return g_pfnCanShowScoreBoard(this, 0);
}

void CCounterStrikeViewport::HideAllVGUIMenu(void)
{
}

void CCounterStrikeViewport::UpdateSpectatorPanel(void)
{
	return g_pfnUpdateSpectatorPanel(this, 0);
}

bool CCounterStrikeViewport::IsScoreBoardVisible(void)
{
	return g_pfnIsScoreBoardVisible(this, 0);
}

void CCounterStrikeViewport::HideScoreBoard(void)
{
	g_pfnHideScoreBoard(this, 0);
}

int CCounterStrikeViewport::KeyInput(int down, int keynum, const char *pszCurrentBinding)
{
	return g_pfnKeyInput(this, 0, down, keynum, pszCurrentBinding);
}

void CCounterStrikeViewport::ShowVGUIMenu(int iMenu)
{
	if (g_pViewPort->ShowVGUIMenu(iMenu))
		return;
	g_pfnShowVGUIMenu(this, 0, iMenu);
}

void CCounterStrikeViewport::HideVGUIMenu(int iMenu)
{
	if (g_pViewPort->HideVGUIMenu(iMenu))
		return;
	g_pfnHideVGUIMenu(this, 0, iMenu);
}

void CCounterStrikeViewport::ShowTutorTextWindow(const wchar_t *szString, int id, int msgClass, int isSpectator)
{
	g_pfnShowTutorTextWindow(this, 0, szString, id, msgClass, isSpectator);
}

void CCounterStrikeViewport::ShowTutorLine(int entindex, int id)
{
	g_pfnShowTutorLine(this, 0, entindex, id);
}

void CCounterStrikeViewport::ShowTutorState(const wchar_t *szString)
{
	g_pfnShowTutorState(this, 0, szString);
}

void CCounterStrikeViewport::CloseTutorTextWindow(void)
{
	g_pfnCloseTutorTextWindow(this, 0);
}

bool CCounterStrikeViewport::IsTutorTextWindowOpen(void)
{
	return g_pfnIsTutorTextWindowOpen(this, 0);
}

void CCounterStrikeViewport::ShowSpectatorGUI(void)
{
	g_pfnShowSpectatorGUI(this, 0);
}

void CCounterStrikeViewport::ShowSpectatorGUIBar(void)
{
	g_pfnShowSpectatorGUIBar(this, 0);
}

void CCounterStrikeViewport::HideSpectatorGUI(void)
{
	g_pfnHideSpectatorGUI(this, 0);
}

void CCounterStrikeViewport::DeactivateSpectatorGUI(void)
{
	g_pfnDeactivateSpectatorGUI(this, 0);
}

bool CCounterStrikeViewport::IsSpectatorGUIVisible(void)
{
	return g_pfnIsSpectatorGUIVisible(this, 0);
}

bool CCounterStrikeViewport::IsSpectatorBarVisible(void)
{
	return g_pfnIsSpectatorBarVisible(this, 0);
}

int CCounterStrikeViewport::MsgFunc_ResetFade(const char *pszName, int iSize, void *pbuf)
{
	return g_pfnMsgFunc_ResetFade(this, 0, pszName, iSize, pbuf);
}

void CCounterStrikeViewport::SetSpectatorBanner(const char *image)
{
	g_pfnSetSpectatorBanner(this, 0, image);
}

void CCounterStrikeViewport::SpectatorGUIEnableInsetView(int value)
{
	g_pfnSpectatorGUIEnableInsetView(this, 0, value);
}

void CCounterStrikeViewport::ShowCommandMenu(void)
{
	g_pfnShowCommandMenu(this, 0);
}

void CCounterStrikeViewport::UpdateCommandMenu(void)
{
	g_pfnUpdateCommandMenu(this, 0);
}

void CCounterStrikeViewport::HideCommandMenu(void)
{
	g_pfnHideCommandMenu(this, 0);
}

int CCounterStrikeViewport::IsCommandMenuVisible(void)
{
	return g_pfnIsCommandMenuVisible(this, 0);
}

int CCounterStrikeViewport::GetValidClasses(int iTeam)
{
	return g_pfnGetValidClasses(this, 0, iTeam);
}

int CCounterStrikeViewport::GetNumberOfTeams(int iTeam)
{
	return g_pfnGetNumberOfTeams(this, 0, iTeam);
}

bool CCounterStrikeViewport::GetIsFeigning(void)
{
	return g_pfnGetIsFeigning(this, 0);
}

int CCounterStrikeViewport::GetIsSettingDetpack(void)
{
	return g_pfnGetIsSettingDetpack(this, 0);
}

int CCounterStrikeViewport::GetBuildState(void)
{
	return g_pfnGetBuildState(this, 0);
}

int CCounterStrikeViewport::IsRandom(void)
{
	return g_pfnIsRandom(this, 0);
}

char *CCounterStrikeViewport::GetTeamName(int iTeam)
{
	return g_pfnGetTeamName(this, 0, iTeam);
}

int CCounterStrikeViewport::GetCurrentMenu(void)
{
	return g_pfnGetCurrentMenu(this, 0);
}

const char *CCounterStrikeViewport::GetMapName(void)
{
	return g_pfnGetMapName(this, 0);
}

const char *CCounterStrikeViewport::GetServerName(void)
{
	return g_pfnGetServerName(this, 0);
}

void CCounterStrikeViewport::InputPlayerSpecial(void)
{
	g_pfnInputPlayerSpecial(this, 0);
}

void CCounterStrikeViewport::OnTick(void)
{
	g_pfnOnTick(this, 0);
}

int CCounterStrikeViewport::GetViewPortScheme(void)
{
	return g_pViewPort->GetScheme();
}

vgui::VPANEL CCounterStrikeViewport::GetViewPortPanel(void)
{
	return g_pViewPort->GetVPanel();
}

int CCounterStrikeViewport::GetAllowSpectators(void)
{
	return g_pfnGetAllowSpectators(this, 0);
}

void CCounterStrikeViewport::OnLevelChange(void)
{
	g_pfnOnLevelChange(this, 0);
}

void CCounterStrikeViewport::HideBackGround(void)
{
	g_pfnHideBackGround(this, 0);
}

void CCounterStrikeViewport::ChatInputPosition(int *x, int *y)
{
	g_pfnChatInputPosition(this, 0, x, y);
}

int CCounterStrikeViewport::GetSpectatorBottomBarHeight(void)
{
	return g_pfnGetSpectatorBottomBarHeight(this, 0);
}

int CCounterStrikeViewport::GetSpectatorTopBarHeight(void)
{
	return g_pfnGetSpectatorTopBarHeight(this, 0);
}

bool CCounterStrikeViewport::SlotInput(int iSlot)
{
	return g_pfnSlotInput(this, 0, iSlot);
}

VGuiLibraryTeamInfo_t CCounterStrikeViewport::GetPlayerTeamInfo(int playerIndex)
{
	return g_pfnGetPlayerTeamInfo(this, 0, playerIndex);
}

void CCounterStrikeViewport::MakeSafeName(const char *oldName, char *newName, int newNameBufSize)
{
	g_pfnMakeSafeName(this, 0, oldName, newName, newNameBufSize);
}

extern void (__fastcall *g_pfnCCounterStrikeViewport_StartProgressBar)(void *pthis, int, const char *title, int numTicks, int startTicks, bool isTimeBased);
extern void (__fastcall *g_pfnCCounterStrikeViewport_UpdateProgressBar)(void *pthis, int, const char *statusText, int tick);
extern void (__fastcall *g_pfnCCounterStrikeViewport_StopProgressBar)(void *pthis, int);

void __fastcall CCounterStrikeViewport_StartProgressBar(void *pthis, int, const char *title, int numTicks, int startTicks, bool isTimeBased)
{
	return g_pfnStartProgressBar(pthis, 0, title, numTicks, startTicks, isTimeBased);
}

void __fastcall CCounterStrikeViewport_UpdateProgressBar(void *pthis, int, const char *statusText, int tick)
{
	return g_pfnUpdateProgressBar(pthis, 0, statusText, tick);
}

void __fastcall CCounterStrikeViewport_StopProgressBar(void *pthis, int)
{
	return g_pfnStopProgressBar(pthis, 0);
}

void CounterStrikeViewport_InstallHook(void)
{
	DWORD *pVFTable = *(DWORD **)&g_CounterStrikeViewport;

	g_pCounterStrikeViewpot = gViewPortInterface;
	g_pMetaHookAPI->VFTHook(g_pCounterStrikeViewpot, 0, 2, (void *)pVFTable[2], (void *&)g_pfnUpdateScoreBoard);
	g_pMetaHookAPI->VFTHook(g_pCounterStrikeViewpot, 0, 4, (void *)pVFTable[4], (void *&)g_pfnGetAllPlayersInfo);
	g_pMetaHookAPI->VFTHook(g_pCounterStrikeViewpot, 0, 6, (void *)pVFTable[6], (void *&)g_pfnShowScoreBoard);
	g_pMetaHookAPI->VFTHook(g_pCounterStrikeViewpot, 0, 8, (void *)pVFTable[8], (void *&)g_pfnHideAllVGUIMenu);
	g_pMetaHookAPI->VFTHook(g_pCounterStrikeViewpot, 0, 9, (void *)pVFTable[9], (void *&)g_pfnUpdateSpectatorPanel);
	g_pMetaHookAPI->VFTHook(g_pCounterStrikeViewpot, 0, 10, (void *)pVFTable[10], (void *&)g_pfnIsScoreBoardVisible);
	g_pMetaHookAPI->VFTHook(g_pCounterStrikeViewpot, 0, 11, (void *)pVFTable[11], (void *&)g_pfnHideScoreBoard);
	g_pMetaHookAPI->VFTHook(g_pCounterStrikeViewpot, 0, 13, (void *)pVFTable[13], (void *&)g_pfnShowVGUIMenu);
	g_pMetaHookAPI->VFTHook(g_pCounterStrikeViewpot, 0, 14, (void *)pVFTable[14], (void *&)g_pfnHideVGUIMenu);
	g_pMetaHookAPI->VFTHook(g_pCounterStrikeViewpot, 0, 20, (void *)pVFTable[20], (void *&)g_pfnShowSpectatorGUI);
	g_pMetaHookAPI->VFTHook(g_pCounterStrikeViewpot, 0, 21, (void *)pVFTable[21], (void *&)g_pfnShowSpectatorGUIBar);
	g_pMetaHookAPI->VFTHook(g_pCounterStrikeViewpot, 0, 22, (void *)pVFTable[22], (void *&)g_pfnHideSpectatorGUI);
	g_pMetaHookAPI->VFTHook(g_pCounterStrikeViewpot, 0, 23, (void *)pVFTable[23], (void *&)g_pfnDeactivateSpectatorGUI);
	g_pMetaHookAPI->VFTHook(g_pCounterStrikeViewpot, 0, 24, (void *)pVFTable[24], (void *&)g_pfnIsSpectatorGUIVisible);
	g_pMetaHookAPI->VFTHook(g_pCounterStrikeViewpot, 0, 25, (void *)pVFTable[25], (void *&)g_pfnIsSpectatorBarVisible);
	g_pMetaHookAPI->VFTHook(g_pCounterStrikeViewpot, 0, 27, (void *)pVFTable[27], (void *&)g_pfnSetSpectatorBanner);
	g_pMetaHookAPI->VFTHook(g_pCounterStrikeViewpot, 0, 28, (void *)pVFTable[28], (void *&)g_pfnSpectatorGUIEnableInsetView);
	g_pMetaHookAPI->VFTHook(g_pCounterStrikeViewpot, 0, 29, (void *)pVFTable[29], (void *&)g_pfnShowCommandMenu);
	g_pMetaHookAPI->VFTHook(g_pCounterStrikeViewpot, 0, 30, (void *)pVFTable[30], (void *&)g_pfnUpdateCommandMenu);
	g_pMetaHookAPI->VFTHook(g_pCounterStrikeViewpot, 0, 31, (void *)pVFTable[31], (void *&)g_pfnHideCommandMenu);
	g_pMetaHookAPI->VFTHook(g_pCounterStrikeViewpot, 0, 32, (void *)pVFTable[32], (void *&)g_pfnIsCommandMenuVisible);
	g_pMetaHookAPI->VFTHook(g_pCounterStrikeViewpot, 0, 45, (void *)pVFTable[45], (void *&)g_pfnGetViewPortScheme);
	g_pMetaHookAPI->VFTHook(g_pCounterStrikeViewpot, 0, 46, (void *)pVFTable[46], (void *&)g_pfnGetViewPortPanel);
	g_pMetaHookAPI->VFTHook(g_pCounterStrikeViewpot, 0, 51, (void *)pVFTable[51], (void *&)g_pfnGetSpectatorBottomBarHeight);
	g_pMetaHookAPI->VFTHook(g_pCounterStrikeViewpot, 0, 52, (void *)pVFTable[52], (void *&)g_pfnGetSpectatorTopBarHeight);

	g_pMetaHookAPI->InlineHook(g_pfnCCounterStrikeViewport_StartProgressBar, CCounterStrikeViewport_StartProgressBar, (void *&)g_pfnCCounterStrikeViewport_StartProgressBar);
	g_pMetaHookAPI->InlineHook(g_pfnCCounterStrikeViewport_UpdateProgressBar, CCounterStrikeViewport_UpdateProgressBar, (void *&)g_pfnCCounterStrikeViewport_UpdateProgressBar);
	g_pMetaHookAPI->InlineHook(g_pfnCCounterStrikeViewport_StopProgressBar, CCounterStrikeViewport_StopProgressBar, (void *&)g_pfnCCounterStrikeViewport_StopProgressBar);
}