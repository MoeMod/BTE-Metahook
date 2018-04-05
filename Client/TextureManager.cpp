/*
*	TextureManager.cpp
*	Replace textures using TextureManager()->ReplaceTexture().
*	Hand tattoo will be done soon.
*	ltndkl
*	2017/9/26 Xiaobaibai: changed to std::map
*	2017/9/26 Xiaobaibai: changed imagelib to LoadTexture
*/
#include <algorithm>
#include <utility>
#include <metahook.h>
#include <plugins.h>
#include <studio.h>
//#include <imagelib.h>
#include <exportfuncs.h>
#include <parsemsg.h>
#include <util.h>
#include <qgl.h>
#include <LoadTexture.h>
#include "TextureManager.h"
#include "iniparser.h"
#include "STLHelper.h"
#include "vgui_controls/Controls.h"


class CErrorTexture : public CTextureManager::CTextureDetail
{
public:
	CErrorTexture()
	{
	}
};

CTextureManager &TextureManager(void)
{
	static CTextureManager x;
	return x;
}

CTextureManager::CTextureManager(void)
{
	m_TexturesNameList.clear();
	m_TexturesIdList.clear();
	m_TexturesReplaceList.clear();

	LoadTextureReplaceList();
	
	m_pErrorTexture = std::make_unique<CErrorTexture>();
}

void CTextureManager::CTextureDetail::Draw(int x, int y, Color rgba, int w, int h) const
{
	if (!w) w = m_width;
	if (!h) h = m_height;
	return GL_DrawTexture(m_idx, rgba.r(), rgba.g(), rgba.b(), rgba.a(), x, y, w, h);
}

CTextureManager::CTextureDetail::~CTextureDetail()
{
}

CTextureManager::~CTextureManager(void)
{
	//for (auto pair : m_Textures)
	//	delete pair.second;

	m_TexturesNameList.clear();
	m_TexturesIdList.clear();
	m_TexturesReplaceList.clear();
}

void CTextureManager::LoadTextureReplaceList()
{
	for (auto &app : CIniParser("textures.ini"))
	{
		std::unordered_map<std::string, std::shared_ptr<CTextureDetail>> KeyList;

		std::string szLowerAppName = ToLower(app.first);

		for (auto &kv : app.second)
		{
			std::string szLowerKey = ToLower(kv.first);
			std::string szLowerValue = ToLower(kv.second);

			auto ptr = GetTexturePtrByName(szLowerValue);
			if (ptr)
			{
				KeyList[szLowerKey] = ptr;
			}
		}

		if (!KeyList.empty())
			m_TexturesReplaceList[szLowerAppName] = std::move(KeyList);
	}
}

auto CTextureManager::GetTexturePtrByName(const std::string &szPath) -> std::shared_ptr<CTextureDetail>
{
	std::string strName = szPath;
	std::transform(strName.begin(), strName.end(), strName.begin(), ::tolower);

	auto it = m_TexturesNameList.find(strName);
	if (it != m_TexturesNameList.end())
	{
		auto ptr = it->second;
		//ptr->Update();
		return ptr;
	}
		
	// NOT FOUND ??? Load it
	auto ptr = FactoryTexture(szPath);
	return std::move(ptr);
}

auto CTextureManager::FactoryTexture(const std::string &szPath) -> std::shared_ptr<CTextureDetail>
{
	const char *szExt = FileExtension(szPath.c_str());
	if (!stricmp(szExt, "bik"))
	{
		auto ptr = std::shared_ptr<CBinkDetail>(new CBinkDetail(szPath));
		if (!ptr || !*ptr)
		{
			return nullptr;
		}
		GLuint iTexIndex = *ptr;
		m_TexturesNameList[szPath] = ptr;
		m_TexturesIdList[iTexIndex] = ptr;
		LogToFile("GetTexturePtrByName() : Load Bink Texture %s", szPath);
		return std::move(ptr);
	}
	else
	{
		int w, h;
		GLuint iTexIndex = GL_LoadTexture(szPath.c_str(), &w, &h);
		if (!iTexIndex)
		{
			return nullptr;
		}

		// due to private constructor
		//auto ptr = std::make_shared<CTextureDetail>(iTexIndex, w, h, szPath);
		auto ptr = std::shared_ptr<CTextureDetail>(new CTextureDetail(iTexIndex, w, h, szPath));
		m_TexturesNameList[szPath] = ptr;
		m_TexturesIdList[iTexIndex] = ptr;
		LogToFile("GetTexturePtrByName() : Load Texture %s", szPath);
		return std::move(ptr);
	}
	
	return nullptr;
}

auto CTextureManager::GetTexturePtrById(int id) -> std::shared_ptr<CTextureDetail>
{
	auto it = m_TexturesIdList.find(id);
	if (it != m_TexturesIdList.end())
	{
		auto ptr = it->second;
		return ptr;
	}

	return nullptr;
}

auto CTextureManager::GetReplacedTexture(char *key, char *name) -> const CTextureDetail &
{
	std::string strKey = key;
	std::string strName = name;
	std::transform(strKey.begin(), strKey.end(), strKey.begin(), ::tolower);
	std::transform(strName.begin(), strName.end(), strName.begin(), ::tolower);
	auto it = m_TexturesReplaceList.find(strKey);
	if (it != m_TexturesReplaceList.end())
	{
		auto it2 = it->second.find(strName);
		if (it2 != it->second.end())
			return *it2->second;
	}

	return *m_pErrorTexture;
}

auto CTextureManager::ReplaceTexture(char *modelname, mstudiotexture_t *p) -> bool
{
	if (!p)
		return 0;
	
	char szModelName[256];

	strcpy(szModelName, modelname);

	if (strlen(szModelName) > 11)
	{
		szModelName[strlen(szModelName) - 4] = '\0';
		const CTextureDetail &Texture = GetReplacedTexture(szModelName + 7, p->name);
		if (Texture)
		{
			qglDeleteTextures(1, (GLuint *)&p->index);
			p->index = Texture;
			p->width = Texture.w();
			p->height = Texture.h();
			strcpy(p->name, Texture.name().c_str());

			return 1;
		}
		
	}
	
	
	const CTextureDetail &Texture = GetReplacedTexture("All", p->name);
	if (Texture)
	{
		qglDeleteTextures(1, (GLuint *)&p->index);
		p->index = Texture;
		p->width = Texture.w();
		p->height = Texture.h();
		strcpy(p->name, Texture.name().c_str());

		return 1;
	}
	

	// Automatically load external texture (probably it's NOT loaded)
	if (p->name[0] == '@' || p->name[0] == '#')
	{
		char szPath[256];
		sprintf(szPath, "models\\texture\\%s", p->name);
		auto ptr = GetTexturePtrByName(szPath);
		if (ptr)
		{
			//LogToFile("ReplaceTexture(): %s(%s) Replaced to %s", p->name, modelname, ptr->name());
			qglDeleteTextures(1, (GLuint *)&p->index);

			p->index = ptr->idx();
			p->width = ptr->w();
			p->height = ptr->h();
			//strcpy(p->name, ptr->name().c_str());

			return 1;
		}
	}

	return 0;
}