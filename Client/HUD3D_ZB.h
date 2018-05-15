struct SkillIcon
{
	//colorVec color;
	float flEndTime;
	float time;
	int type;
	int  texture;
};

class CHud3D_ZB
{
	friend class CHudZombieMod3;
public:
	void Init(void);
	void VidInit(void);
	void Redraw(float flTime);
	void Reset();
	void RoundStart();
	void SetAdd(int i, float f);
	void SetAtk(int i, float f);
	void SetSkill(char *pszName, int iSlot, int type, float time = 0.0F);
private:
	void SetBar(float atk, float def, float max);

	void CalcPos(int i, int width, int height);
	void Think(float flTime);
	void DrawBg();
	void DrawSkill(float flTime);
	void DrawBar();
	void Draw(int id, int i, int w, int h, float xoff, float yoff);
	void Draw(int id, int i, int w, int h, float pos[4][2], float xoff, float yoff);
	void DrawFinal(int id, Vector origin[4]);
	void DrawFinal(int id, float pos[4][2], Vector origin[4]);
	void LoadTexture(char *name, int &id, wrect_t &prc);
public:

private:
	int m_iBg[4];
	wrect_t m_rcBg[2];
	int m_iNumber[10];
	wrect_t m_rcNumber[10];

	int m_iBar;
	wrect_t m_rcBar;

	Vector m_vecOrigin[2][4];

	int m_iHelmet;
	wrect_t m_rcHelemet;

	float m_flPrecent[2];

	float m_flNextCalc;

	float m_flMax;
	float m_flAtk[2];
	float m_flAdd[2];

	SkillIcon m_icon[4];
};

extern CHud3D_ZB gHud3D_ZB;

