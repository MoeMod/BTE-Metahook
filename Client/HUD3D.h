class CHud3D
{
public:
	void Init(void);
	void VidInit(void);
	void Redraw(float flTime);
	void ResetHPAP(float delay = 0.0f) { m_flNextReset = gEngfuncs.GetAbsoluteTime() + delay; }
	void SetMapBg(char *pszPath);
private:
	void Think(float flTime);
	void CalcPos(int i, int w, int h);
	void DrawNumbers();
	void DrawNumber(int i, int num, float sclae, float interval, float xoff, float yoff, bool right_start = false, int len = 0, bool draw_zero = false);
	void DrawHPAP();
	void DrawBg();
	void DrawMap();
	void DrawFlashlight();
	void Draw(int id, int i, int w, int h, float xoff, float yoff);
	void Draw(int id, int i, int w, int h, float pos[4][2], float xoff, float yoff);
	void DrawFinal(int id, Vector origin[4]);
	void DrawFinal(int id, float pos[4][2], Vector origin[4]);
	void LoadTexture(char *name, int &id, wrect_t &rc);

	void CalcPoint(const float* origin, int &x, int &y);
public:
	int m_iMaxHPAP[2];
	int m_iHPAP[2];
private:
	int m_iBg[3];
	int m_iMapBg;
	int m_iMapBg2;
	wrect_t m_rcBg[2];
	int m_iNumber[10];
	wrect_t m_rcNumber[10];
	
	Vector m_vecOrigin[2][4];

	int m_iLine;
	wrect_t m_rcLine;
	int m_iDollar[3];
	wrect_t m_rcDollar;
	int m_iReloading;
	wrect_t m_rcReloading;
	
	colorVec m_colorReload;
	float m_fReloadTimer;

	colorVec m_colorMoney;
	int m_iOldMoney;
	int m_iMoney;
	float m_fMoneyTimer;

	int m_iHPAPBar;
	wrect_t m_rcHPAPBar;
	colorVec m_colorHP;
	float m_flHPTimer;
	int m_iHPFade;

	int m_iFlashlightBar;
	wrect_t m_rcFlashlightBar;

	int m_iMapIcon[5];
	wrect_t m_rcMapIcon[5];

	float m_flNextReset;

	int m_iHelmet;
	wrect_t m_rcHelemet;
};

extern CHud3D gHud3D;