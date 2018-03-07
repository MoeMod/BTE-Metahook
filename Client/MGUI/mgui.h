#pragma once

#define MGUI_PANEL	1
#define MGUI_IMAGE	2
#define MGUI_TEXTENTRY 3
#define MGUI_MENU	4
#define MGUI_BUTTON	5
#define MGUI_LABEL	6

#define MGUI_BUTTON_TYPE_COMMAND	1
#define MGUI_BUTTON_TYPE_CHANGE_NAME	2
#define MGUI_BUTTON_TYPE_COMMAND_CLOSEPANEL	3
#define MGUI_BUTTON_TYPE_CLOSEALL	4
#define MGUI_BUTTON_TYPE_FUNCTION	5
#define MGUI_BUTTON_TYPE_FUNCTION_CLOSEPANEL	6
#define MGUI_BUTTON_TYPE_FUNCTION_CLOSEALL		7

#define ME_LEFTCLICK	1
#define ME_RIGHTCLICK	2
#define ME_WHEELCLICK	4

#define MGUI_TEX_TYPE_1		1
#define MGUI_TEX_TYPE_3		2
#define MGUI_TEX_TYPE_9		3

#define MGUI_Left		1
#define MGUI_Center		2
#define MGUI_Right		3

class MGUI_Base
{
public:
	int x,y,w,h;
	int prevx,prevy;
	int iClass;
	MGUI_Base *pParent;
	MGUI_Base *pNext;

	void(*pfnPrePaint)(MGUI_Base *control) = NULL;
	void(*pfnPostPaint)(MGUI_Base *control) = NULL;

	void *pSaveData = NULL;	// Used for developers to save own values or structs
public:
	virtual void Paint(MGUI_Base *pParent);
};

class MGUI_MSGBox;
class MGUI_Button;

class MGUI_Panel:public MGUI_Base
{
public:
	bool m_bCanMove;
	int	m_iTexture[3][3];
	int m_iTitleHeight;
	int m_iClosing; // Draw
	int m_bHasClose;
	int m_bCanControl;
	int m_iHaveBg;
	wchar_t m_pTitle[64];
	int m_iTitleSize;
	bool m_bCenterTitle;
	bool m_bPerformingMessageBox;
	MGUI_Panel *pNextPanel;
	MGUI_MSGBox *m_pMsgBox = NULL;

public :
	MGUI_Panel();
	bool InsertControl(MGUI_Base *pChild);
	int SetTexture(int iTex[3][3]);
	bool IsInRect(void);
	bool IsPerformingMessageBox(void)
	{
		return m_bPerformingMessageBox;
	}
	char *GetTextEntryText(int iClass);

	void MGUIMessageBox(wchar_t szCaption[], wchar_t szText[], int iType, void(*pfnOKClick)(MGUI_Button *) = NULL, void (*pfnCancelClick)(MGUI_Button *) = NULL, int iOKKey = 0, int iCancelKey = 0);

public :
	~MGUI_Panel();
};

class MGUI_Image:public MGUI_Base
{
public :
	int m_iTexture;
	int	m_iTexture3[3];
	int	m_iTexture9[3][3];
	bool m_bAutoResize;
	bool m_bForceSize;
	bool m_bVisible;
	int m_iTex_Type;
public :
	void SetTexture(char *pszName);
	void SetVisible(bool bVisible);
	virtual void Paint(MGUI_Base *pParent);
};

class MGUI_TextEntry:public MGUI_Base
{
public :
	bool m_bCanType;
	bool m_bHasFocus;
	char m_pText[256];
	int m_iClass;
public :
	wchar_t *GetText(void);
	void SetText(char *pText);
	bool IsInRect(void);
	virtual void Paint(MGUI_Base *pParent);
};

class MGUI_Menu:public MGUI_Base
{
public :
	bool m_bShow;
	int m_iNum;
	char pText[10][100];
	int m_iOnFocus;
public :
	virtual void Paint(MGUI_Base *pParent);
	int IsInRect(void);
	void AddItem(char *pItemName);
};
class MGUI_Button:public MGUI_Base
{
public :
	int m_iStat;
	int m_iType;
	int m_iType2;
	char pszCommand[256];
	char pszCommand2[256];
	wchar_t pszLabel[32];
	float m_fNextClick;
	int iTga[3];
	int iTga3[3][3];
	int iTga9[3][3][3];
	bool m_bVisible;
	bool m_bEnabled;
	bool m_bEnabled2;
	int m_iSize;
	int m_Tex_Type;
	MGUI_Panel *m_OverPanel;
	int m_iColor32[4];
	int m_iOffset[2];
	int m_iKey;
	BOOL m_bClickable;

	void(*pfnClick)(MGUI_Button *pButton) = NULL;
	void(*pfnClick2)(MGUI_Button *pButton) = NULL;

public :
	virtual void Paint(MGUI_Base *pParent);
	bool IsInRect(void);
	void Click(void);
	void Click2(void);
	void SetType(int iType);
	void SetCommand(char *pCommand);
	void SetCommand2(char *pCommand);
	void SetLabel(wchar_t *pLabel);
	void SetImage(char *n, char *o, char *c);
	void SetImage3(int iTex[3][3]);
	void SetVisible(bool bVisible);
	void SetEnabled(bool bEnabled);
	void SetColor(int r,int g,int b,int a);
	void SetKey(int iKey);

	void SetClickable(BOOL state) { m_bClickable = state; }
	BOOL Clickable(void) { return m_bClickable; }
};
class MGUI_Label:public MGUI_Base
{
public :
	wchar_t pszLabel[512];
	int m_iColor32[4];
	int m_iSize;
	bool m_bVisible;
	int m_iAlignment;
	int m_iType, m_iMaxHeight;
public:
	void SetColor(int r,int g,int b,int a);
	void SetLabel(wchar_t *pLabel);
	void SetSize(int iSize);
	virtual void Paint(MGUI_Base *pParent);
	void SetVisible(bool bVisible);
};
class MGUI_MSGBox
{
public:
	MGUI_MSGBox();
public:
	MGUI_Panel *m_pPanel = NULL;
	MGUI_Button *m_pButtonOK = NULL, *m_pButtonCancel = NULL;
	MGUI_Label *m_pLabelText = NULL;
};

void MGUI_Redraw();
void MGUI_Init();
void MGUI_ProcessMessage(void *);
char *MGUI_FindFocusTextEntryText(void);
void MGUI_SetFocusTextEntryDisabled(void);
void MGUI_BTE_CreateConfigPanel(void);
extern int g_mgui_mouseevent,g_mgui_oldmouseevent,g_mgui_candraw,g_mgui_keynum,g_mgui_mouse;
extern int g_mouse_w, g_mouse_h;
extern float g_mgui_nextclicktime;

MGUI_Panel *MGUI_CreatePanel(int x, int y, int w, int h, int iTitleHeight, bool CanMove, bool bHasMsgBox = false);
MGUI_Label *MGUI_CreateLabel(MGUI_Base *pParent,int x,int y,int w,int h);
MGUI_Image *MGUI_CreateImage(MGUI_Base *pParent,int x,int y,int w,int h,bool AutoResize,bool ForceSize);
MGUI_TextEntry *MGUI_CreateTextEntry(MGUI_Base *pParent,int x,int y,int w);
MGUI_Button *MGUI_CreateButton(MGUI_Base *pParent,int x,int y,int w,int h);
MGUI_MSGBox *MGUI_CreateMessageBox(MGUI_Panel *pParent, wchar_t caption[], wchar_t text[], int iType, void(*pfnOKClick)(MGUI_Button *) = NULL, void(*pfnCancelClick)(MGUI_Button *) = NULL, int iOKKey = 0, int iCancelKey = 0);