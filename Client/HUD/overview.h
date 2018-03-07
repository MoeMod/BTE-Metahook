#pragma once

class CHudOverview : public CHudBase
{
public:
	void Init(void);

public:
	void VidInit(void);
	void EntityESP(void);
	void DrawEntities(void);
	bool CanDraw(void);
	void CalcRefdef(struct ref_params_s *pParams);
	void Draw(float time);
	int CalcRadarPoint(const float* origin, int& screenx, int& screeny, int iCheck, int scale);
	void SetBombStatus(int iEntity, float fTime) { m_iBombEntity = iEntity; m_fBombEntityTime = fTime; }
public:
	bool m_bCanCheck, m_bCanDraw;
	model_t *m_pSprite;
	float m_flZoom;
	int m_iHeight;
	Vector2D m_vecOrigin;
	bool m_bIsRotate;
	model_t *m_pCamera, *m_rgpPlayer[3], *m_pDeath, *m_pInfection, *m_pRadio, *m_pBackPack, *m_pHostage, *m_pC4, *m_pVIP;
	cvar_t *cl_radaron;
	float m_fViewAngles_SinYaw, m_fViewAngles_CosYaw;
	int m_iMaxClient;

	bool m_bCanSee[33];

	int m_iBombEntity;
	float m_fBombEntityTime;
};

CHudOverview &HudOverview();