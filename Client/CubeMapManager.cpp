
#include "BaseUI.h"
#include "studio.h"
#include "util.h"
#include "common.h"
#include <io.h>

#include "exportfuncs.h"
#include "triangleapi.h"

#include <cvardef.h>
#include <qgl.h>
#include <gl/glext.h>
#include <pbo.h>
#include "CubeMapManager.h"
#include "LoadTGA.h"

CCubeMapManager gCubeMapManager;

char *g_envsuf[6] = { "rt", "bk", "lf", "ft", "up", "dn" };

CCubeMapManager::CCubeMapManager()
{
	for (int i = 0; i < MAX_SHADER_ENV; i++)
	{
		m_pszEnvName[i] = NULL;
	}
	for (int i = 0; i < MAX_SHADER_CUBEMAP; i++)
	{
		m_pCubeMapData[i] = NULL;
	}
}

CCubeMapManager::~CCubeMapManager()
{
	ClearData();
}

void CCubeMapManager::Reset()
{
	ClearData();
	LoadData();
}

void CCubeMapManager::ClearData()
{
	for (int i = 0; i < MAX_SHADER_ENV; i++)
	{
		delete[] m_pszEnvName[i];
		m_pszEnvName[i] = NULL;
	}
		
	for (int i = 0; i < MAX_SHADER_CUBEMAP; i++)
	{
		delete m_pCubeMapData[i];
		m_pCubeMapData[i] = NULL;
	}
		
}

void CCubeMapManager::LoadData()
{
	FILE *fp = NULL;
	if (!(fp = fopen("cstrike\\shader\\env.txt", "r")))
		return;
	int iCachedID;
	char szBuffer[256];
	
	while (fscanf(fp, "%d %s", &iCachedID, szBuffer) != EOF)
	{
		if (iCachedID >= MAX_SHADER_ENV || iCachedID < 0)
			continue;
		delete[] m_pszEnvName[iCachedID];
		m_pszEnvName[iCachedID] = new char[strlen(szBuffer) + 1];
		strcpy(m_pszEnvName[iCachedID], szBuffer);

		
		qglGenTextures(1, &m_EnvTextureId[iCachedID]);
		qglBindTexture(GL_TEXTURE_CUBE_MAP, m_EnvTextureId[iCachedID]);

		for (int j = 0; j < 6; j++)
		{
			int iTexIndex = vgui::surface()->CreateNewTextureID();
			sprintf(szBuffer, "gfx\\env\\%s%s.tga", m_pszEnvName[iCachedID], g_envsuf[j]);

			int w, h;
			if (!GetTGASize(szBuffer, &w, &h))
				continue;
			int buffersize = w * h * 4;

			m_EnvTextures[iCachedID][j].data.reset(new byte[buffersize]);
			LoadTGA(szBuffer, m_EnvTextures[iCachedID][j].data.get(), buffersize, &w, &h);

			m_EnvTextures[iCachedID][j].w = w;
			m_EnvTextures[iCachedID][j].h = w;

			qglTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X_EXT + j,
				0,                  //level 
				GL_RGB8,            //internal format 
				w,                 //width 
				h,                 //height 
				0,                  //border 
				GL_RGBA,             //format 
				GL_UNSIGNED_BYTE,   //type 
				m_EnvTextures[iCachedID][j].data.get()); // pixel data 
		}

		qglBindTexture(GL_TEXTURE_CUBE_MAP, 0);
		//gEngfuncs.pfnConsolePrint(va("Add env [%d] %s \n", iCachedID, m_pszEnvName[iCachedID]));
	}

	fclose(fp);

	_finddata_t fileDir;
	long lfDir;
	if ((lfDir = _findfirst(".\\cstrike\\shader\\@???.txt", &fileDir)) != -1l)
	{
		do
		{
			if (sscanf(fileDir.name, "@%3d.txt", &iCachedID) != 1)
				continue;
			if (iCachedID >= MAX_SHADER_CUBEMAP || iCachedID < 0)
				continue;
			if (!(fp = fopen(va(".\\cstrike\\shader\\%s", fileDir.name), "r")))
				continue;
			
			delete m_pCubeMapData[iCachedID];
			m_pCubeMapData[iCachedID] = new CubeMapData;

			for (int i = 0; i < 4; i++)
			{
				if (feof(fp))
					break;
				fgets(szBuffer, 255, fp);

				if (szBuffer[0] == '0')
				{
					sscanf(szBuffer, "%*d%*s%d", &(m_pCubeMapData[iCachedID]->iEnvID));
				}
				if (szBuffer[0] == '1')
				{
					sscanf(szBuffer, "%*d%*s%f", &(m_pCubeMapData[iCachedID]->mulBright));
				}
				if (szBuffer[0] == '2')
				{
					sscanf(szBuffer, "%*d%*s%f", &(m_pCubeMapData[iCachedID]->addBright));
				}
			}

			fclose(fp);
			//gEngfuncs.pfnConsolePrint(va("Add cubemap [%d] %d %.1f %.1f \n", iCachedID, m_pCubeMapData[iCachedID]->iEnvID, m_pCubeMapData[iCachedID]->mulBright, m_pCubeMapData[iCachedID]->addBright));

		} while (_findnext(lfDir, &fileDir) == 0);
	}
	_findclose(lfDir);
}

extern mstudiotexture_t gCurrentTexture;

bool CCubeMapManager::CheckTexture()
{
	/*int iCubeMapID = 0;
	if (sscanf(gCurrentTexture.name + 3, "@%d", &iCubeMapID) < 1)
		return false;
	
	if (iCubeMapID>=10)
		return false;*/
	
	return true;
}


void CCubeMapManager::SetupTexture()
{
	int iCubeMapID = 3;
	
	qglEnable(GL_TEXTURE_CUBE_MAP);
	qglEnable(GL_TEXTURE_CUBE_MAP_EXT);

	auto textureID = m_EnvTextureId[iCubeMapID];
	qglBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	//glTexEnvi(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_ENV, GL_REPLACE);
	qglTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	qglTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	/*qglTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_REPEAT);
	qglTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_REPEAT);
	qglTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_REPEAT);*/

	qglTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_EXT);
	qglTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_EXT);
	qglTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_EXT);
	qglEnable(GL_TEXTURE_GEN_S);
	qglEnable(GL_TEXTURE_GEN_T);
	qglEnable(GL_TEXTURE_GEN_R);

	//qglDisable(GL_TEXTURE_CUBE_MAP);
	//auto &Texture = *m_EnvTexture[iCubeMapID][0];
	//qglBindTexture(GL_TEXTURE_2D, Texture);
}


void CCubeMapManager::UnloadTexture()
{
	qglDisable(GL_TEXTURE_GEN_S);
	qglDisable(GL_TEXTURE_GEN_T);
	qglDisable(GL_TEXTURE_GEN_R);

	qglBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	qglDisable(GL_TEXTURE_CUBE_MAP_EXT);
	qglDisable(GL_TEXTURE_CUBE_MAP);
}