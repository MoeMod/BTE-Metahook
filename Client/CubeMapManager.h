#pragma once

#define MAX_SHADER_ENV 10
#define MAX_SHADER_CUBEMAP 10

#include <memory>

typedef unsigned int GLuint;

class CCubeMapManager
{
public:
	CCubeMapManager();
	~CCubeMapManager();
	void Reset();
	bool CheckTexture();
	void SetupTexture();
	void UnloadTexture();

private:
	void ClearData();
	void LoadData();

public:
	char *m_pszEnvName[MAX_SHADER_ENV];
	struct CubeMapTextureDetail
	{
		std::unique_ptr<byte[]> data;
		int w;
		int h;
	};
	GLuint m_EnvTextureId[MAX_SHADER_ENV];
	CubeMapTextureDetail m_EnvTextures[MAX_SHADER_ENV][6];

	struct CubeMapData
	{
		int iEnvID;
		float mulBright;
		float addBright;
	};
	CubeMapData *m_pCubeMapData[MAX_SHADER_CUBEMAP];
};

extern CCubeMapManager gCubeMapManager;