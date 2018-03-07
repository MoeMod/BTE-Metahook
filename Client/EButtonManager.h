#pragma once

class CEButtonManager
{
public:
	void Init(void);
	void AddAttachment(int entindex, int iTeamLimit, int bOn);
	void RemoveAttachment(int entindex);

private:
	TEMPENTITY *m_pAttachments[512];
	model_t *m_pModel;
};

extern CEButtonManager TheEButtons;