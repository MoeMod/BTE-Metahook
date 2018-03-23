
#include "client.h"
#include "exportfuncs.h"
#include "EButtonManager.h"

TEMPENTITY *CL_DefaultSprite(const vec3_t pos, model_t *sprite, float framerate);

CEButtonManager TheEButtons;

void ATTACHMENT_EBUTTON_CALLBACK(TEMPENTITY *ent, float frametime, float currenttime)
{
	static cl_entity_t *pl, *pEntity;
	pl = gEngfuncs.GetLocalPlayer();
	pEntity = gEngfuncs.GetEntityByIndex(ent->entity.curstate.iuser1);

	if (!pEntity)
	{
		TheEButtons.RemoveAttachment(ent->entity.curstate.iuser1);
		return;
	}
	ent->entity.origin = pEntity->origin + Vector(0, 0, 25);

	if (pl && vPlayer[pl->index].team == ent->entity.curstate.iuser2)
		TheEButtons.RemoveAttachment(ent->entity.curstate.iuser1);
}

void CEButtonManager::Init(void)
{
	memset(m_pAttachments, NULL, sizeof(m_pAttachments));

	m_pModel = IEngineStudio.Mod_ForName("sprites/e_button01.spr", FALSE);
}

void CEButtonManager::AddAttachment(int entindex, int iTeamLimit, int bOn)
{
	if (!bOn)
		return RemoveAttachment(entindex);
	
	if (entindex >= 512 || entindex <= 0)
	{
		gEngfuncs.Con_DPrintf("AddAttachment : Invalid entity %d!", entindex);
		return;
	}
	if (iTeamLimit > 0)
	{
		if (gEngfuncs.GetLocalPlayer() && vPlayer[gEngfuncs.GetLocalPlayer()->index].team == iTeamLimit)
		{
			return;
		}
	}
	cl_entity_t *pEntity = gEngfuncs.GetEntityByIndex(entindex);
	if (!pEntity)
		return;

	m_pAttachments[entindex] = CL_DefaultSprite(pEntity->origin + Vector(0, 0, 25), m_pModel, 0);
	if (m_pAttachments[entindex])
	{
		m_pAttachments[entindex]->flags &= ~FTENT_SPRANIMATE;
		m_pAttachments[entindex]->flags |= FTENT_CLIENTCUSTOM;
		m_pAttachments[entindex]->entity.curstate.rendermode = kRenderTransAdd;
		m_pAttachments[entindex]->entity.curstate.renderamt = 200;
		m_pAttachments[entindex]->entity.curstate.scale = 0.12;
		m_pAttachments[entindex]->entity.curstate.iuser1 = entindex;
		m_pAttachments[entindex]->entity.curstate.iuser2 = iTeamLimit;
		m_pAttachments[entindex]->callback = ATTACHMENT_EBUTTON_CALLBACK;
		m_pAttachments[entindex]->die = cl.time + 9999.0f;
	}
}

void CEButtonManager::RemoveAttachment(int entindex)
{
	if (entindex >= 512 || entindex <= 0)
	{
		gEngfuncs.Con_DPrintf("RemoveAttachment : Invalid entity %d!", entindex);
		return;
	}
	if (m_pAttachments[entindex])
	{
		m_pAttachments[entindex]->die = cl.time;
		m_pAttachments[entindex] = NULL;
	}
}