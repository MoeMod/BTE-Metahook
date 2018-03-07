#include "base.h"
#include "client.h"
#include "exportfuncs.h"
#include "BaseUI.h"
#include "svc_hook.h"
#include "sizebuf.h"
#include "parsemsg.h"
#include "util.h"

#define CL_DUMPMESSAGELOAD_F_SIG "\x55\x8B\xEC\x83\xE4\xF8\x51\x53\x56\x57\x68"
#define CL_DUMPMESSAGELOAD_F_SIG_NEW "\x55\x8B\xEC\x51\x53\x56\x57\x68\x2A\x2A\x2A\x2A\x33\xDB\xE8\x2A\x2A\x2A\x2A\x83\xC4\x04\x33\xF6\xBF\x2A\x2A\x2A\x2A\x8B\x04\xB5\x2A\x2A\x2A\x2A"
int SVC_LASTMSG = 50;

typedef struct svc_func_s
{
	unsigned char opcode;
	char *pszname;
	pfnSVC_Parse pfnParse;
}
svc_func_t;

svc_func_t *cl_parsefuncs = NULL;
sizebuf_t *net_message = NULL;
int *msg_readcount = NULL;

pfnSVC_Parse g_pfnParseWeaponAnim;
pfnSVC_Parse g_pfnParseTempEntity;

void ENTAttachment_Callback(tempent_s *ent, float frametime, float currenttime)
{
	static cl_entity_t *viewent;
	viewent = gEngfuncs.GetViewModel();

	if (!viewent || !viewent->model || !strstr(viewent->model->name, "v_dragonsword"))
	{
		ent->die = currenttime;
		return;
	}

	ent->entity.origin = viewent->origin;// + Vector(0, 0, 16);
	ent->entity.angles = viewent->angles;
}

void ENTAttachment_Callback2(tempent_s *ent, float frametime, float currenttime)
{
	static cl_entity_t *viewent;
	viewent = gEngfuncs.GetViewModel();

	if (!viewent || !viewent->model || !strstr(viewent->model->name, "v_dragonsword"))
	{
		ent->die = currenttime;
		return;
	}

	if ((currenttime - ent->entity.baseline.fuser1) >= 16.0f / 30.0f)
	{
		if (!(ent->flags & FTENT_SPRANIMATE))
		{
			ent->flags |= FTENT_SPRANIMATE;
			ent->entity.curstate.animtime = currenttime;
			ent->entity.curstate.framerate = 1.0f;
			ent->entity.curstate.frame = 0.0f;
			ent->entity.curstate.sequence = !(gEngfuncs.pfnGetCvarFloat("cl_righthand") > 0);
			ent->entity.baseline.rendercolor.g = 128;
			ent->fadeSpeed = 256;
			ent->frameMax = 8;
		}
		ent->entity.curstate.renderamt = 128;
		ent->entity.origin = viewent->origin;
		ent->entity.angles = viewent->angles;
	}
}

TEMPENTITY *CL_TempModel(vec3_t pos, vec3_t dir, vec3_t angles, float life, model_t *mod, int soundtype)
{
	// alloc a new tempent
	TEMPENTITY	*pTemp;

	pTemp = gEngfuncs.pEfxAPI->CL_TempEntAlloc(pos, mod);
	if (!pTemp) return NULL;

	// keep track of shell type
	switch (soundtype)
	{
	case TE_BOUNCE_SHELL:
		pTemp->hitSound = BOUNCE_SHELL;
		break;
	case TE_BOUNCE_SHOTSHELL:
		pTemp->hitSound = BOUNCE_SHOTSHELL;
		break;
	}

	VectorCopy(pos, pTemp->entity.origin);
	VectorCopy(angles, pTemp->entity.angles);
	VectorCopy(dir, pTemp->entity.baseline.origin);

	pTemp->entity.curstate.body = 0;
	pTemp->flags = (FTENT_COLLIDEWORLD | FTENT_GRAVITY | FTENT_ROTATE);
	pTemp->entity.baseline.angles[0] = gEngfuncs.pfnRandomFloat(-255, 255);
	pTemp->entity.baseline.angles[1] = gEngfuncs.pfnRandomFloat(-255, 255);
	pTemp->entity.baseline.angles[2] = gEngfuncs.pfnRandomFloat(-255, 255);
	pTemp->entity.curstate.rendermode = kRenderNormal;
	pTemp->entity.baseline.renderamt = 255;
	pTemp->die = cl.time + life;

	return pTemp;
}

extern float g_flBloodhunterAnimTime;
extern int g_iBloodhunterSecAnim;
extern double g_dbStormgiantEffectTime;

void ParseWeaponAnim(void)
{
	BEGIN_READ(SVC_GetBuffer(), SVC_GetBufferSize());

	int iAnim = READ_BYTE();
	int body = READ_BYTE();

	static cl_entity_t *ent;
	ent = gEngfuncs.GetViewModel();

	if (ent && ent->model)
	{
		if (g_iBTEWeapon == WPN_BLOODHUNTER)
		{
			if (iAnim < 3)
			{
				if (!g_flBloodhunterAnimTime)
					g_flBloodhunterAnimTime = cl.time;
				g_iBloodhunterSecAnim = 19 + (g_iWeaponStat - 50);
			}
			else
			{
				g_flBloodhunterAnimTime = 0.0;
				g_iBloodhunterSecAnim = 0;
			}
		}
		else
		{
			g_flBloodhunterAnimTime = 0.0;
			g_iBloodhunterSecAnim = 0;
		}
		
		if (g_iBTEWeapon == WPN_STORMGIANT)
		{
			g_dbStormgiantEffectTime = cl.time;
		}

		if (strstr(ent->model->name, "v_dragonsword.mdl"))
		{
			/*static*/ model_t *mod = IEngineStudio.Mod_ForName("models/ef_dragonsword.mdl", FALSE);
			if (iAnim == 5)
			{
				TEMPENTITY *te = CL_TempModel(ent->origin, Vector(0, 0, 0), Vector(0, 0, 0), 1.0, mod, 0);//TEMPENTITY *te = gEngfuncs.pEfxAPI->CL_TempEntAlloc(ent->origin, mod);
				if (te)
				{
					te->flags &= ~(FTENT_COLLIDEWORLD | FTENT_GRAVITY);
					te->flags |= FTENT_CLIENTCUSTOM | FTENT_SPRANIMATE | FTENT_FADEOUT;
					te->entity.angles = ent->angles;
					te->tentOffset = Vector(0, 0, 16);
					te->clientIndex = ent->index;
					te->entity.curstate.skin = 0;
					te->entity.curstate.body = 0;
					te->bounceFactor = 3;	// ??
					te->entity.curstate.rendermode = kRenderTransAdd;
					te->entity.curstate.renderamt = 128;
					te->entity.baseline.rendercolor.g = 128;
					te->entity.baseline.angles = Vector(0, 0, 0);
					te->fadeSpeed = 256;
					te->callback = ENTAttachment_Callback;
					te->entity.curstate.sequence = /*gEngfuncs.pfnGetCvarFloat("cl_righthand") > 0 ?*/ 2 /*: 3*/;
					te->entity.curstate.frame = 0.0;
					te->entity.curstate.framerate = 1;
					te->entity.curstate.animtime = cl.time;
					te->frameMax = 15;
				}
			}
			else if (iAnim == 1)
			{
				TEMPENTITY *te = CL_TempModel(ent->origin + Vector(0, 0, 16), Vector(0, 0, 0), Vector(0, 0, 0), 1.0, mod, 0);//gEngfuncs.pEfxAPI->CL_TempEntAlloc(ent->origin, mod);
				if (te)
				{
					te->flags &= ~(FTENT_COLLIDEWORLD | FTENT_GRAVITY);
					te->flags |= FTENT_CLIENTCUSTOM | FTENT_FADEOUT;
					te->entity.angles = ent->angles;
					te->entity.curstate.rendermode = kRenderTransAdd;
					te->tentOffset = Vector(0, 0, 16);
					te->clientIndex = ent->index;
					te->entity.curstate.skin = 0;
					te->entity.curstate.body = 0;
					te->bounceFactor = 3;	// ??
					te->entity.curstate.renderamt = 0.0;
					te->entity.baseline.fuser1 = cl.time;
					te->entity.baseline.rendercolor.r = 128;
					te->fadeSpeed = 256;
					te->entity.baseline.angles = Vector(0, 0, 0);
					te->callback = ENTAttachment_Callback2;
					te->entity.curstate.sequence = 0; // !(gEngfuncs.pfnGetCvarFloat("cl_righthand") > 0);
				}
			}
		}
	}

	return g_pfnParseWeaponAnim();
}

void ParseTempEntity(void)
{
	BEGIN_READ(SVC_GetBuffer(), SVC_GetBufferSize());

	int iTEType = READ_BYTE();
	

	return g_pfnParseTempEntity();
}

void SVC_InstallHooks(void)
{
	g_pfnParseWeaponAnim = SVC_HookFunc(svc_weaponanim, ParseWeaponAnim);
	g_pfnParseTempEntity = SVC_HookFunc(svc_tempentity, ParseTempEntity);
}

void SVC_Init(void)
{
	DWORD addr_svc_bad = (DWORD)g_pMetaHookAPI->SearchPattern((void *)g_dwEngineBase, g_dwEngineSize, "svc_bad", sizeof("svc_bad") - 1);
	DWORD addr_svc_nop = (DWORD)g_pMetaHookAPI->SearchPattern((void *)addr_svc_bad, 0x100, "svc_nop", sizeof("svc_nop") - 1);
	
	int svc[6] = { 0, addr_svc_bad, 0, 1, addr_svc_nop, 0 };
	char *svc_sig = (char *)svc;

	DWORD addr_cl_parsefuncs = (DWORD)g_pMetaHookAPI->SearchPattern((void *)g_dwEngineBase, g_dwEngineSize, svc_sig, sizeof(svc));
	
	if (addr_cl_parsefuncs)
	{	
		cl_parsefuncs = (svc_func_t *)addr_cl_parsefuncs;

		while (SVC_LASTMSG)
		{
			if (cl_parsefuncs[SVC_LASTMSG].opcode == 0xFF)
			{
				SVC_LASTMSG -= 1;
				break;
			}

			SVC_LASTMSG++;
		}
	}

	DWORD addr_cl_parse_director = (DWORD)cl_parsefuncs[51].pfnParse;
	if (addr_cl_parse_director)
	{
		msg_readcount = (int *)(*(DWORD *)(addr_cl_parse_director + 8));
		net_message = (sizebuf_t *)(*(DWORD *)(addr_cl_parse_director + 15) - 0x8);
	}

	SVC_InstallHooks();
}

pfnSVC_Parse SVC_HookFunc(int opcode, pfnSVC_Parse pfnParse)
{
	pfnSVC_Parse pfnResult = cl_parsefuncs[opcode].pfnParse;

	cl_parsefuncs[opcode].pfnParse = pfnParse;
	return pfnResult;
}

void *SVC_GetBuffer(void)
{
	return net_message->data + *msg_readcount;
}

int SVC_GetBufferSize(void)
{
	return net_message->cursize - *msg_readcount;
}

void SVC_Skip(int size)
{
	*msg_readcount += size;
}