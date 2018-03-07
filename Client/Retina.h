#pragma once

class CHudRetinaObjects
{
public:
	void Init();
	void VidInit(char *filename, bool drawmode, bool flash);
	void SetCanDraw(bool draw){m_bCanDraw = draw;}
	void SetEndTime(float time) {m_flEndTime = time;}
	void SetDrawMode(bool drawmode, bool flash)
	{
		m_bFlash = flash;
		m_bDrawMode = drawmode;
	}
	void SetTexture(int id) {m_iTextureId = id;}
	void Redraw(float flTime);
	void SetColor(int r, int g, int b, int a)
	{
		m_color[0] = r;
		m_color[1] = g;
		m_color[2] = b;
		m_color[3] = a;

		m_color[3] = m_color[3] > 255 ? 255 : m_color[3];
		m_color[3] = m_color[3] < 0 ? 0 : m_color[3];
	}

private:
	int m_iTextureId;

	bool m_bCanDraw;
	bool m_bFlash;
	bool m_bDrawMode;

	float m_flEndTime;

	wrect_t m_rcTexture;
	
	int m_color[4];
};