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
	std::vector<TenviSkill> skill;
	std::vector<BYTE> titles;

	TenviCharacter(std::wstring name, std::wstring profile, DWORD id, BYTE job_mask, WORD job, WORD skin,
		WORD hair, WORD face, WORD gcolor, BYTE awakening, WORD map, BYTE level, WORD sp, WORD ap,
		WORD stat_str, WORD stat_dex, WORD stat_hp, WORD stat_int, WORD stat_mp, WORD maxHP, WORD HP, WORD maxMP,
		WORD MP, BYTE titleEquipped, DWORD money);

	bool UseSP(WORD skill_id);
	bool UseAP(BYTE stat_id, BYTE amount);
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

	std::map<WORD, std::vector<TenviSkill>> AwakeningMap = {
		{100, { {31, 10}, {32, 10}, {20035, 10}, {20036, 10}, {40035, 10}, {40036, 10}, }}, // 어택 머신		로그			엘리멘탈 위자드
		{200, { {33, 10}, {34, 10}, {20037, 10}, {20038, 10}, {40037, 10}, {40038, 10}, }}, // 슈팅 머신		디버퍼			프레이어
		{111, { {35, 10}, {36, 10}, {20039, 10}, {20040, 10}, {40039, 10}, {40040, 10}, }}, // 투핸드 머신		대거 블레이더	파이어 메이지
		{121, { {37, 10}, {38, 10}, {20041, 10}, {20042, 10}, {40041, 10}, {40042, 10}, } }, // 원핸드 머신		클로 블레이더	콜드 메이지
		{211, { {39, 10}, {40, 10}, {20043, 10}, {20044, 10}, {40043, 10}, {40044, 10}, }}, // 스피드 머신		데빌 위치		홀리 메이지
		{221, { {41, 10}, {42, 10}, {20045, 10}, {20046, 10}, {40045, 10}, {40046, 10}, }}, // 마인 머신		다크 위치		프리스트
		{112, { {43, 3}, {44, 10}, {20047, 3}, {20048, 10}, {40051, 3}, {40052, 10}, }},	 // 배틀 머신		대거 어쌔신		파이어 소서러
		{122, { {43, 3}, {45, 10}, {20047, 3}, {20049, 10}, {40053, 3}, {40054, 10}, }},    // 쉴드 머신		클로 어쌔신		콜드 소서러
		{212, { {46, 3}, {47, 10}, {20050, 3}, {20051, 10}, {40055, 3}, {40056, 10}, }},    // 스나이프 머신	데빌 서머너		홀리 서머너
		{222, { {46, 3}, {48, 10}, {20050, 3}, {20052, 10}, {40055, 3}, {40057, 10}, }},    // 캐논 머신		다크 헥스		하이 프리스트
		{113, { {56, 10}, {57, 10}, {20024, 10}, {20029, 10}, {40021, 10}, {40026, 10}, }}, // 버서크 머신		대거 어벤져		파이어 마스터
		{123, { {58, 10}, {59, 10}, {20030, 10}, {20061, 10}, {40029, 10}, {40031, 10}, }}, // 가드 머신		클로 어벤져		콜드 마스터
		{213, { {60, 10}, {61, 10}, {20062, 10}, {20063, 10}, {40067, 10}, {40068, 10}, }}, // 데스 머신		데빌 마스터		홀리 마스터
		{223, { {62, 10}, {63, 10}, {20064, 10}, {20065, 10}, {40069, 10}, {40070, 10}, }}, // 로켓 머신		다크 마스터		아크 프리스트
		{114, { {64, 10}, {65, 10}, {20066, 10}, {20067, 10}, {40071, 10}, {40072, 10}, }}, // 디스트로이어		스위시 버클러	불의 화신
		{124, { {66, 10}, {67, 10}, {20068, 10}, {20069, 10}, {40073, 10}, {40074, 10}, }}, // 프로텍터			슬레이어		얼음의 화신
		{214, { {68, 10}, {69, 10}, {20070, 10}, {20071, 10}, {40075, 10}, {40076, 10}, }}, // 데스페라도		이모탈			이블 져지
		{224, { {70, 10}, {71, 10}, {20072, 10}, {20073, 10}, {40077, 10}, {40078, 10}, }}  // 테크니션			카오스			빛의 화신
	};		;
	std::vector<TenviSkill> GetAwakening(BYTE job, WORD awakening);
};

#endif