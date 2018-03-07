class CHudAmmo : public CHudBase
{
public:
	void Init(void);
	void VidInit(void);
	void Draw(float flTime);

public:
	void CalculateCrosshairColor(void);
	void CalculateCrosshairDrawMode(void);
	void CalculateCrosshairSize(void);
	void SetNvgOn(bool bNvg);

	int GetWeaponAccuracyFlags(int iWeaponID);

	int DrawCrosshair(float flTime, int weaponid);
	int DrawCrosshairEx(float flTime, int weaponid, int iBarSize, float flCrosshairDistance, bool bAdditive, int r, int g, int b, int a);
	inline void Hit(float flTime)
	{
		m_flLastHitTime = flTime;
	}
	inline void HitForBuff(float flTime)
	{
		m_flLastBuffHit = flTime + 0.1;
	}

public:
	void DrawAmmo(float time);
	void DrawExtraAmmo(float time);

public:
	int m_iAmmo[32];
	int m_iExtraAmmo;
	float m_fFade;

private:
	bool m_bNvgOn;
	int m_HUD_bucket0;
	int m_HUD_selection;
	HSPRITE m_hObserverCrosshair, m_hBuffHit, m_hInfinite;
	wrect_t m_rcObserverCrosshair, m_rcInfinite;
	int m_bObserverCrosshair;
	int m_iAmmoLastCheck;
	int m_iBuffHitWidth, m_iBuffHitHeight;
	float m_flCrosshairDistance;
	int m_iAlpha;
	int m_R;
	int m_G;
	int m_B;
	int m_cvarR;
	int m_cvarG;
	int m_cvarB;
	int m_iCurrentCrosshair;
	bool m_bAdditive;
	int m_iCrosshairScaleBase;
	char m_szLastCrosshairColor[32];
	float m_flLastCalcTime;
	float m_flLastHitTime, m_flLastBuffHit;
};

CHudAmmo &HudAmmo();