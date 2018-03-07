#pragma once

#include <unordered_map>

class CPlayerClassManager
{
public:
	enum Sex_e
	{
		SEX_NONE,
		SEX_MALE,
		SEX_FEMALE,
	};

	struct ClassData
	{
		//wchar_t name[64];
		char model[32];
		Sex_e sex;
		int team;
		char szTextureReplaceKey[32];

		explicit operator bool()
		{
			return model[0] != '\0';
		}
	};

	friend CPlayerClassManager &PlayerClassManager();
	ClassData &OnPrecacheClass(const char *szName);
	ClassData &Find(const char *szName);
	inline void Reset()
	{
		for (auto pair : m_DataMap)
			delete pair.second;

		m_DataMap.clear();
	}
	inline ClassData &GetPlayerClass(int idx)
	{
		if (!m_PlayerClass[idx])
			return m_NullClass;
		return *m_PlayerClass[idx];
	}
	inline ClassData &SetPlayerClass(int idx, const char *szName)
	{
		return SetPlayerClass(idx, Find(szName));
	}
	inline ClassData &SetPlayerClass(int idx, ClassData &Other)
	{
		m_PlayerClass[idx] = &Other;
		return *m_PlayerClass[idx];
	}
	inline ClassData &operator[](const char *szName) { return Find(szName); }
	inline ClassData &operator[](int idx) { return GetPlayerClass(idx); }
	//inline std::unordered_map<std::string, ClassData *> &GetList() { return m_DataMap; }
	// supports Range-based for()
	struct FakeIterator
	{
		std::unordered_map<std::string, ClassData *>::iterator m_it;
		inline FakeIterator(std::unordered_map<std::string, ClassData *>::iterator it)
		{
			m_it = it;
		}
		inline bool operator!=(const FakeIterator &Other)
		{
			return m_it != Other.m_it;
		}
		inline ClassData &operator*()
		{
			return *m_it->second;
		}
		inline const FakeIterator &operator++()
		{
			m_it++;
			return *this;
		}
	};
	inline FakeIterator begin() { return m_DataMap.begin(); }
	inline FakeIterator end() { return m_DataMap.end(); }

private:
	CPlayerClassManager()
	{
		//m_pCurWeapon = nullptr;
		memset(&m_NullClass, 0, sizeof(m_NullClass));

		for (auto x : m_PlayerClass)
			x = 0;
	}
	CPlayerClassManager(CPlayerClassManager &Other);
	CPlayerClassManager(CPlayerClassManager &&Other);
	~CPlayerClassManager()
	{
		for (auto pair : m_DataMap)
			delete pair.second;

		m_DataMap.clear();
	}

	static ClassData LoadClassData(const char *szName);

	ClassData *m_PlayerClass[33];
	ClassData m_NullClass;
	std::unordered_map<std::string, ClassData *> m_DataMap;
};

CPlayerClassManager &PlayerClassManager();

char *AliasClassName(char *name);
wchar_t *GetClassNameFormat(char *name);
wchar_t *GetClassInfoFormat(char *name);