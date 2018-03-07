#pragma once

typedef struct line
{
	wchar_t text[1024];
	float die;
	int team;
	int height;

	struct line* next;
};

class CHudSayText : public CHudBase
{
public:
	CHudSayText()
	{
		this->head = NULL;
	}

	void MsgFunc_SayText(const char *pszName, int iSize, void *pbuf);
	void MsgFunc_TextMsg(const char *pszName, int iSize, void *pbuf);
	void Add(wchar_t *text, int team);
	void Draw(float flTime);
	void Think();
	void VidInit();
	void Init();
	~CHudSayText();

public:
	int *m_bInputing;
	int m_bInputCheck;

	int m_iListCount;
	int m_iSelection;
	int m_iInsertPos;
	int m_iIMEMaxWidth;

	wchar_t m_wszList[512][1024];
	wchar_t m_wszComp[1024];

	line *head;

private:

	int m_iFontSize;
	int m_iLineSpace;
	int m_iMaxWidth;
	float m_flScale;
	int m_iMaxLine;
};

CHudSayText &HudSayText();