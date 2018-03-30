#pragma once

#include <vector>

class CPrecacheManager
{
public:
	friend CPrecacheManager &PrecacheManager();

	void Init();
	void VidInit();
	void OnPrecacheModel(const char *szFileName);

	std::vector<std::string> m_vecModelPrecache;
};

//void PrecacheManager_InstallHook();