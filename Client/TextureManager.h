#pragma once

#include <map>
#include <vector>
#include <functional>
#include <memory>
#include <unordered_map>
#include <Color.h>
#include <GL_BinkTexture.h>
#include "HLSDK/engine/studio.h"

/*
*	TextureManager.h
*	Definitions of CTextureManager class and other
*	ltndkl
*/

class CTextureManager
{
public:
	friend CTextureManager &TextureManager(void);
	class CTextureDetail
	{
	public:
		friend CTextureManager;
		operator int() const { return m_idx; }
		inline int idx() const { return m_idx; }
		inline int w() const { return m_width; }
		inline int h() const { return m_height; }
		inline const std::string &name() const { return m_szName; }
		void Draw(int x, int y, Color rgba = Color(255, 255, 255, 255), int w = 0, int h = 0) const;

		virtual ~CTextureDetail();

	protected:
		CTextureDetail(const std::string &name = "") : m_szName(name)
		{
			m_idx = 0;
			m_width = 0;
			m_height = 0;
		}
		CTextureDetail(int index, int w, int h, const std::string &name) : CTextureDetail(name)
		{
			m_idx = index;
			m_width = w;
			m_height = h;
		}
		CTextureDetail(CTextureDetail &other) = delete;
		CTextureDetail(CTextureDetail &&other) = delete;

		int m_idx, m_width, m_height;
		std::string m_szName;
	};

	class CBinkDetail : public CTextureDetail
	{
		friend CTextureManager;
	public:
		void Update() const
		{
			BinkTexture.UpdateFrame();
		}
	protected:
		CBinkDetail(const std::string &name) : BinkTexture(name.c_str()), CTextureDetail(name)
		{
			m_idx = BinkTexture.GetTextureId();
			m_width = BinkTexture.w();
			m_height = BinkTexture.h();
			TextureManager().AddUpdateFunc(std::bind(&CBinkDetail::Update, this));
		}
		mutable CGL_BinkTexture BinkTexture;
	};

private:
	CTextureManager();
	CTextureManager(CTextureManager &Other);
	CTextureManager(CTextureManager &&Other);
	~CTextureManager();

	
public:

	void LoadTextureReplaceList();
	bool ReplaceTexture(char *modelname, mstudiotexture_t *p);
	const CTextureDetail &GetReplacedTexture(char *key, char *name);
	const CTextureDetail &operator[](char *szPath) 
	{ 
		auto ptr = GetTexturePtrByName(szPath);
		if (!ptr) 
			return *m_pErrorTexture;
		return *ptr;
	}
	const CTextureDetail &operator[](int id)
	{
		auto ptr = GetTexturePtrById(id);
		if (!ptr) 
			return *m_pErrorTexture;
		return *ptr;
	}
	void UpdateAll() const
	{
		for (auto &f : m_UpdateFuncs)
			f();
	}
	void AddUpdateFunc(std::function<void()> f)
	{
		m_UpdateFuncs.emplace_back(f);
	}

private:
	std::shared_ptr<CTextureDetail> FactoryTexture(const std::string &szPath);
	std::shared_ptr<CTextureDetail> GetTexturePtrByName(const std::string &szPath);
	std::shared_ptr<CTextureDetail> GetTexturePtrById(int id);

	std::unordered_map<std::string, std::unordered_map<std::string, std::shared_ptr<CTextureDetail>>> m_TexturesReplaceList;
	std::unordered_map<std::string, std::shared_ptr<CTextureDetail>> m_TexturesNameList;
	std::unordered_map<int, std::shared_ptr<CTextureDetail>> m_TexturesIdList;
	std::unique_ptr<CTextureDetail> m_pErrorTexture;
	std::vector<std::function<void()>> m_UpdateFuncs;
};

CTextureManager &TextureManager(void);