#ifndef __DATABASE_H__
#define __DATABASE_H__

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
	DWORD inventoryID;
	WORD itemID;
	BYTE loc;
	BYTE type;
	BYTE group;
	BYTE slot;
	BYTE rank;
	BYTE isCash;
	DWORD price;
	WORD number;
	BYTE isEquip;
} Item;

enum TenviStat {
	TS_STR,
	TS_DEX,
	TS_HP,
	TS_INT,
	TS_MP,
};

enum ItemSlot {
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

	// data
	float x;
	float y;
	bool isLoaded;

	BYTE guardian_flag;
	std::wstring profile;
	std::vector<BYTE> titles;

	TenviCharacter(std::wstring name, std::wstring profile, DWORD id, BYTE job_mask, WORD job, WORD skin,
		WORD hair, WORD face, WORD gcolor, BYTE awakening, WORD map, BYTE level, WORD sp, WORD ap,
		WORD stat_str, WORD stat_dex, WORD stat_hp, WORD stat_int, WORD stat_mp, WORD maxHP, WORD HP, WORD maxMP,
		WORD MP, BYTE titleEquipped, DWORD money);

	bool UseSP(WORD skill_id);
	bool UseAP(BYTE stat_id, BYTE amount);
	std::vector<TenviSkill> GetSkill();
	std::map<BYTE, Item> GetEquipped(BYTE isCash);
	std::vector<Item> GetInventory();
	Item GetItemByLoc(BYTE type, BYTE loc);
	Item GetItemBySlot(BYTE slot, BYTE isCash);
	int GetEmptyLoc(BYTE type);
	Item GetItemByInventoryID(DWORD inventoryID);
	Item GetItemByItemID(WORD itemID);
	void ChangeItemLoc(DWORD inventoryID, BYTE loc);
	void DeleteItem(DWORD inventoryID);
	void ChangeItemNumber(DWORD inventoryID, WORD number);
	void SetHP(WORD hp);
	void SetMP(WORD mp);
	void HealHP(WORD amount);
	void HealMP(WORD amount);
	void KeySet(std::string str);
	std::vector<BYTE> GetKeySet();
	void ChangeTitle(BYTE code);
	void ChangeMoney(DWORD money);
	void ChangeProfile(std::wstring wText);
	void SortInventory(BYTE method, BYTE type);
	void SwitchRing(BYTE isCash);
	void AddItem(Item item);
	void EquipItem(Item item, BYTE ring4 = 0);
	void UnequipItem(Item item, BYTE loc, BYTE ring4 = 0);
	void SetMapReturn(WORD map_return_id);
	void SetMap(WORD map_id);
};

class TenviAccount {
private:
	std::vector<TenviCharacter> characters;
	DWORD online_id;
	DWORD objectCounter;

public:
	BYTE slot;
	static DWORD inventoryCount;
	static std::map<WORD, std::vector<TenviSkill>> AwakeningMap;
	TenviAccount();
	bool FindCharacter(DWORD id, TenviCharacter *found);
	std::vector<TenviCharacter>& GetCharacters();
	bool AddCharacter(std::wstring nName, BYTE nJob_Mask, WORD nJob, WORD nSkin, WORD nHair, WORD nFace, WORD nCloth, WORD nGColor, WORD gHead, WORD gBody, WORD gWeapon);
	static Item MakeItem(TenviCharacter& chr, WORD itemID, WORD number=1);
	DWORD GetObjectID();
	void ClearObjectID();
	bool Login(DWORD id);
	TenviCharacter& GetOnline();
	static DWORD GetHighestInventoryID();
	static std::vector<TenviSkill> GetAwakening(BYTE job, WORD awakening);
	bool DeleteCharacter(DWORD chr_id);
};

#endif