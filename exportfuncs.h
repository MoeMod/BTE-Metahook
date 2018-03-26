#include <com_model.h>
#include <r_studioint.h>

extern cl_enginefunc_t gEngfuncs;
extern event_api_s gEventAPI;
extern efx_api_s gEfxAPI;
extern engine_studio_api_t IEngineStudio;
extern int g_iMod;
extern int g_iUser1;
extern int g_iUser2;
extern int g_iUser3;
extern int g_iTeamNumber;
extern int g_iFreezeTimeOver;
extern int g_iPlayerFlags;
extern float g_flPlayerSpeed;
extern int g_iWeaponFlags;
extern int g_iWeaponLimit;
extern float g_Next_Key_CanUse;
extern int g_iHookVAngle, g_iShowCustomCrosshair;
extern float g_vecVAngle[3];
extern float v_angles[3];
extern float punchangle[3];
extern float g_velocity[3];
extern float vieworg[3];

extern int g_iScreenTexture;

extern int g_bFlashActive;
extern int g_iFlashBat;

extern float g_flTutorClose;

extern int g_iWeaponAnim;

extern bool g_bNvg;
extern int g_iZBNvg;

extern cl_entity_t **r_currententity;

extern int (*Engfunc_Call_AddVisibleEntity)(struct cl_entity_s *pEntity);

int Initialize(struct cl_enginefuncs_s *pEnginefuncs, int iVersion);
void HUD_Init(void);
int HUD_VidInit(void);
int HUD_Redraw(float time, int intermission);
void HUD_DrawTransparentTriangles(void);
void V_CalcRefdef(struct ref_params_s *pParams);
int HUD_GetStudioModelInterface(int iVersion, struct r_studio_interface_s **ppStudioInterface, struct engine_studio_api_s *pEngineStudio);
int HUD_AddEntity(int iType, struct cl_entity_s *pEntity, const char *pszModel);
void HUD_PlayerMove(struct playermove_s *pPlayerMove, int iServer);
int HUD_UpdateClientData(client_data_t *pClientData, float flTime);
void HUD_StudioEvent(const struct mstudioevent_s *pEvent, const struct cl_entity_s *pEntity);
void HUD_PostRunCmd(struct local_state_s *from, struct local_state_s *to, struct usercmd_s *cmd, int runfuncs, double time, unsigned int random_seed);
void HUD_TempEntUpdate(double dblFrameTime, double dblClientTime, double dblGravity, struct tempent_s **ppTempEntFree, struct tempent_s **ppTempEntActive, int (*pfnAddVisibleEntity)(struct cl_entity_s *pEntity), void (*pfnTempEntPlaySound)(struct tempent_s *pTemp, float flDamp));
int HUD_Key_Event(int eventcode, int keynum, const char *pszCurrentBinding);
void HUD_ProcessPlayerState(struct entity_state_s *dst, const struct entity_state_s *src);
void IN_Accumulate(void);
void IN_ClearStates(void);
void IN_MouseEvent(int);
void HUD_CreateEntities(void);
void HUD_DrawNormalTriangles(void);

void Engfunc_PlaySoundByName(char *szSound, float volume);
int Engfunc_DrawConsoleString(int x, int y, char *string);

int Engfunc_VGUI2DrawCharacter(int x, int y, int ch, unsigned int font);


void HUD_TxferPredictionData(struct entity_state_s *ps, const struct entity_state_s *pps, struct clientdata_s *pcd, const struct clientdata_s *ppcd, struct weapon_data_s *wd, const struct weapon_data_s *pwd);
void HUD_TxferLocalOverrides(struct entity_state_s *state, const struct clientdata_s *client);

void CL_CreateMove(float frametime, struct usercmd_s *cmd, int active);

int ModelFrameCount(model_t *mod);