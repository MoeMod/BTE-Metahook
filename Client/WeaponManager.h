#pragma once

#include <unordered_map>

class CWeaponManager
{
public:
	enum WeaponBuyMenuType
	{
		NONE = -1,
		PISTOL = 0,
		SHOTGUN,
		SMG,
		RIFLE,
		MG,
		EQUIP,
		KNIFE
	};

	enum WeaponBuyTeam
	{
		ALL,
		CT,
		TR,
	};

	enum WeaponEffect
	{
		EFFECT_NONE,
		EFFECT_BLOCK_ORIGIN,
		EFFECT_SHAKE,
		EFFECT_SHELL_M134,
		EFFECT_M1887_XMAS,
		EFFECT_TRACER_SFSNIPER,
		EFFECT_TRACER_ETHEREAL
	};
	
	struct CustomWeapon
	{
		/*
		class MoeReturn
		{
		public:
			MoeReturn(char *x) : sz(x) {}
			MoeReturn(std::string &x) { sz = x.c_str(); }
			operator int() { return atoi(sz); }
			operator float() { return atof(sz); }
			operator const char *() { return sz; }
			operator bool()
			{
				if (!stricmp(sz, "TRUE"))
					return true;
				return false;
			}
			const char *sz;
		};

		std::unordered_map<std::string, std::string> KeyValues;
		MoeReturn operator[](const char *key)
		{
			return KeyValues[key];
		}
		*/
		char szName[32];
		char szShell[32];
		float fShellVec[4];
		char szSound[64];
		char szSound2[64];
		char szSoundSilen[64];
		char szSoundSilen2[64];
		//float fSoundVolume;
		//float fSoundAttenuation;
		bool bHaveShellModel;
		bool bHaveShellVec;
		bool bHaveAnim;
		int	iAnimStart;
		int iAnimEnd;
		int	iAnimEmpty;
		int iAnimStart2;
		int iAnimEnd2;
		int iAnimSwing[2];
		int iAnimStabHit[2];
		int iAnimStab[2];
		bool bHaveShotGunSpread;
		float fShotGunSpread[3];
		int iShotGunShots;
		int iEffect;
		bool bRandomSound;
		char szSlash[5][64];
		char szSlashHit[5][64];
		char szWall[5][64];
		char szStab[5][64];
		char szStabHit[5][64];
		char szDraw[64];
		int iSlash;
		int iSlashHit;
		int iStab;
		int iStabHit;
		int iWall;
		int iSniperScope[4];
		int iSniperScopeType;
		char szSniperScope[64];
		bool bCanBuy;
		WeaponBuyTeam iTeam;
		WeaponBuyMenuType iMenu;
		int iSlot;
		int iModeLimit;

		int iPrice;

		int iClip;
		int iAmmo;

		int iViewModelDisplay;

		float flSrcOfs;

		explicit operator bool()
		{
			return szName[0] != '\0';
		}
	};

	friend CWeaponManager &WeaponManager();

	CustomWeapon &OnPrecacheWeapon(const char *szName);

	CustomWeapon &Find(const char *szName);
	inline CustomWeapon &operator[](const char *szName) { return Find(szName); }
	inline void Reset()
	{
		for (auto pair : m_DataMap)
			delete pair.second;

		m_DataMap.clear();
	}
	inline CustomWeapon &GetCurWeapon() 
	{ 
		if (!m_pCurWeapon)
			return m_NullWeapon;
		return *m_pCurWeapon; 
	}
	inline CustomWeapon &SetCurWeapon(const char *szName)
	{
		return SetCurWeapon(Find(szName));
	}
	inline CustomWeapon &SetCurWeapon(CustomWeapon &B)
	{
		m_pCurWeapon = &B;
		return B;
	}

	inline CustomWeapon &GetPlayerWeapon(int idx, int iSlot)
	{
		if (!m_pPlayerWeapon[idx][iSlot])
			return m_NullWeapon;
		return *m_pPlayerWeapon[idx][iSlot];
	}
	inline CustomWeapon &SetPlayerWeapon(int idx, int iSlot, const char *szName)
	{
		return SetPlayerWeapon(idx, iSlot, Find(szName));
	}
	inline CustomWeapon &SetPlayerWeapon(int idx, int iSlot, CustomWeapon &Other)
	{
		m_pPlayerWeapon[idx][iSlot] = &Other;
		return *m_pPlayerWeapon[idx][iSlot];
	}

	//inline std::unordered_map<std::string, CustomWeapon *> &GetList() { return m_DataMap; }
	// supports Range-based for()
	struct FakeIterator
	{
		std::unordered_map<std::string, CustomWeapon *>::iterator m_it;
		inline FakeIterator(std::unordered_map<std::string, CustomWeapon *>::iterator it)
		{
			m_it = it;
		}
		inline bool operator!=(const FakeIterator &Other)
		{
			return m_it != Other.m_it;
		}
		inline CustomWeapon &operator*()
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
	static CustomWeapon LoadWeaponData(const char *szName);
	static WeaponBuyMenuType GetWeaponBuyMenuType(const char *szName);
	CWeaponManager()
	{
		m_pCurWeapon = nullptr;
		memset(&m_NullWeapon, 0, sizeof(m_NullWeapon));
	}
	CWeaponManager(CWeaponManager &Other);
	CWeaponManager(CWeaponManager &&Other);
	~CWeaponManager()
	{
		for (auto pair : m_DataMap)
			delete pair.second;

		m_DataMap.clear();
	}

	CustomWeapon m_NullWeapon;
	CustomWeapon *m_pCurWeapon;
	CustomWeapon *m_pPlayerWeapon[33][5];
	std::unordered_map<std::string, CustomWeapon *> m_DataMap;
};

CWeaponManager &WeaponManager();

char *AliasWeaponName(char *name);
wchar_t *GetWeaponNameFormat(char *name);
wchar_t *GetWeaponDescription(char *pItem);