#pragma once
#include <vector>
#include "Client/PlayerClassManager.h"

extern struct CLASS;

class CCSTeamMenu
{
public:
	CCSTeamMenu()
	{
		m_pPanel = NULL;
		memset(m_pButtonSlot, NULL, sizeof(m_pButtonSlot));
		memset(m_pButtonSlotLabel, NULL, sizeof(m_pButtonSlotLabel));
		memset(m_pButtonSlotKeyImage, NULL, sizeof(m_pButtonSlotKeyImage));
		memset(m_pButtonSlotKeyLabel, NULL, sizeof(m_pButtonSlotKeyLabel));
		memset(m_pButtonTeamSlot, NULL, sizeof(m_pButtonTeamSlot));
		memset(m_pButtonPage, NULL, sizeof(m_pButtonPage));
		memset(m_pPanelInfo, NULL, sizeof(m_pPanelInfo));
		memset(m_pImageInfo, NULL, sizeof(m_pImageInfo));
		memset(m_pLabelInfo, NULL, sizeof(m_pLabelInfo));
		m_iCurrentPage = m_iCurrentSlot = m_iCurrentTeam = 0;
	}
	~CCSTeamMenu()
	{
		delete m_pPanel;
		for (int i = 0; i < 10; i++)
		{
			delete m_pButtonSlot[i];
			delete m_pButtonSlotLabel[i];
			delete m_pButtonSlotKeyImage[i];
			delete m_pButtonSlotKeyLabel[i];
			delete m_pPanelInfo[i];
			delete m_pImageInfo[i];
			delete m_pLabelInfo[i];
		}
		for (int i = 0; i < 2; i++)
		{
			delete m_pButtonTeamSlot[i];
			delete m_pButtonPage[i];
		}
	}
	void Init(void);
	void BuildBaseFrame(void);
	void Reset(void);
	void Show(void);
	void Close(void);

public:
	void UpdateItems(void);
	void Command(char *szCommand);
	void SwitchToPage(unsigned iPage);
	void SwitchTeam(int iTeam);
	
private:
	MGUI_Panel *m_pPanel;

	MGUI_Button *m_pButtonSlot[10];
	MGUI_Label *m_pButtonSlotLabel[10];
	MGUI_Image *m_pButtonSlotKeyImage[10];
	MGUI_Label *m_pButtonSlotKeyLabel[10];

	MGUI_Panel *m_pPanelInfo[10];
	MGUI_Image *m_pImageInfo[10];
	MGUI_Label *m_pLabelInfo[10];

	MGUI_Button *m_pButtonTeamSlot[2];
	MGUI_Button *m_pButtonPage[2];

	

	//int m_iElementCount;
	std::vector<CPlayerClassManager::ClassData *> m_Classes;

	int m_iCurrentTeam, m_iCurrentSlot, m_iCurrentPage;
	int m_iButtonTexture[3][3];
	int m_iColumnTexture[3][3];
};

extern CCSTeamMenu g_TeamMenu;