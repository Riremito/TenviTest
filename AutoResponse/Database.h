#ifndef __TEMPORARYDATA_H__
#define __TEMPORARYDATA_H__

#include<Windows.h>
#include <mysql.h>
#include<string>
#include<vector>
#include<map>

typedef struct{
	WORD id;
	BYTE level;
} TenviSkill;

typedef struct {
	WORD itemID;
	BYTE slot;
	BYTE isCash;
	BYTE type;
	DWORD price;
	WORD number;
	DWORD inventoryID;
} Item;

enum TenviStat {
	TS_STR,
	TS_DEX,
	TS_HP,
	TS_INT,
	TS_MP,
};

enum ItemType {
	cl = 0, // cloth
	cp = 1, // cap
	nc = 2, // necklace
	ri = 3, // ring (can be 4)
	am = 6, // armor
	_do = 7, // head
	wp = 8, // pilot weapon
	lp = 9, // leg
	pp = 10, // wing
	op = 11, // card
	dc = 12, // arm
	rh = 13, // right hand weapon
	lh = 14, // left hand weapon
	th = 13 // two hand weapon
};

class TenviCharacter {
private:
	static DWORD id_counter;

public:
	DWORD id;
	std::wstring name;
	BYTE job_mask;
	WORD job;
	WORD skin;
	WORD hair;
	WORD face;
	WORD cloth;
	WORD gcolor; // guadian
	WORD map;
	WORD map_return;
	BYTE level;
	WORD sp; // skill point
	WORD ap;
	BYTE awakening;
	// stat
	WORD stat_str;
	WORD stat_dex;
	WORD stat_hp;
	WORD stat_int;
	WORD stat_mp;
	WORD maxHP;
	WORD HP;
	WORD maxMP;
	WORD MP;
	BYTE fly;
	BYTE aboard;
	BYTE titleEquipped;
	BYTE direction;
	DWORD money;
	BYTE equipLoc;
	BYTE extraLoc;
	BYTE questLoc;
	BYTE cashLoc;
	BYTE cardLoc;

	// data
	float x;
	float y;

	BYTE guardian_flag;
	std::wstring profile;
	std::map<BYTE, Item> equipped;
	std::map<BYTE, Item> gequipped;
	std::map<BYTE, Item> inventory_equip;
	std::map<BYTE, Item> inventory_extra;
	std::map<BYTE, Item> inventory_quest;
	std::map<BYTE, Item> inventory_cash;
	std::map<BYTE, Item> inventory_card;
	std::vector<TenviSkill> skill;
	std::vector<BYTE> titles;

	TenviCharacter(std::wstring name, std::wstring profile, DWORD id, BYTE job_mask, WORD job, WORD skin,
		WORD hair, WORD face, WORD cloth, WORD gcolor, BYTE awakening, WORD map, BYTE level, WORD sp, WORD ap,
		WORD stat_str, WORD stat_dex, WORD stat_hp, WORD stat_int, WORD stat_mp, WORD maxHP, WORD HP, WORD maxMP,
		WORD MP, BYTE titleEquipped, DWORD money, std::map<BYTE, Item>& nEquipped, std::map<BYTE, Item>& nGEquipped);

	void InitItem();
	void TestSilva();
	bool UseSP(WORD skill_id);
	bool UseAP(BYTE stat_id, BYTE amount);
	void SetMapReturn(WORD map_return_id);
	void SetMap(WORD map_id);
};

class TenviAccount {
private:
	std::vector<TenviCharacter> characters;
	DWORD online_id;

public:
	BYTE slot;
	static DWORD inventoryCount;

	TenviAccount();
	void LateInit();
	bool FindCharacter(DWORD id, TenviCharacter *found);
	std::vector<TenviCharacter>& GetCharacters();
	bool AddCharacter(std::wstring nName, BYTE nJob_Mask, WORD nJob, WORD nSkin, WORD nHair, WORD nFace, WORD nCloth, WORD nGColor, BYTE nAwakening, std::map<BYTE, Item> &nEquipped, std::map<BYTE, Item> &nGEquipped);
	static Item MakeItem(WORD itemID, WORD number=1);
	bool Login(DWORD id);
	TenviCharacter& GetOnline();
};

#endif