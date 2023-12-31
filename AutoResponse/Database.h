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
		{100, { {31, 10}, {32, 10}, {20035, 10}, {20036, 10}, {40035, 10}, {40036, 10}, }}, // ���� �ӽ�		�α�			������Ż ���ڵ�
		{200, { {33, 10}, {34, 10}, {20037, 10}, {20038, 10}, {40037, 10}, {40038, 10}, }}, // ���� �ӽ�		�����			�����̾�
		{111, { {35, 10}, {36, 10}, {20039, 10}, {20040, 10}, {40039, 10}, {40040, 10}, }}, // ���ڵ� �ӽ�		��� ���̴�	���̾� ������
		{121, { {37, 10}, {38, 10}, {20041, 10}, {20042, 10}, {40041, 10}, {40042, 10}, } }, // ���ڵ� �ӽ�		Ŭ�� ���̴�	�ݵ� ������
		{211, { {39, 10}, {40, 10}, {20043, 10}, {20044, 10}, {40043, 10}, {40044, 10}, }}, // ���ǵ� �ӽ�		���� ��ġ		Ȧ�� ������
		{221, { {41, 10}, {42, 10}, {20045, 10}, {20046, 10}, {40045, 10}, {40046, 10}, }}, // ���� �ӽ�		��ũ ��ġ		������Ʈ
		{112, { {43, 3}, {44, 10}, {20047, 3}, {20048, 10}, {40051, 3}, {40052, 10}, }},	 // ��Ʋ �ӽ�		��� ��ؽ�		���̾� �Ҽ���
		{122, { {43, 3}, {45, 10}, {20047, 3}, {20049, 10}, {40053, 3}, {40054, 10}, }},    // ���� �ӽ�		Ŭ�� ��ؽ�		�ݵ� �Ҽ���
		{212, { {46, 3}, {47, 10}, {20050, 3}, {20051, 10}, {40055, 3}, {40056, 10}, }},    // �������� �ӽ�	���� ���ӳ�		Ȧ�� ���ӳ�
		{222, { {46, 3}, {48, 10}, {20050, 3}, {20052, 10}, {40055, 3}, {40057, 10}, }},    // ĳ�� �ӽ�		��ũ ��		���� ������Ʈ
		{113, { {56, 10}, {57, 10}, {20024, 10}, {20029, 10}, {40021, 10}, {40026, 10}, }}, // ����ũ �ӽ�		��� ���		���̾� ������
		{123, { {58, 10}, {59, 10}, {20030, 10}, {20061, 10}, {40029, 10}, {40031, 10}, }}, // ���� �ӽ�		Ŭ�� ���		�ݵ� ������
		{213, { {60, 10}, {61, 10}, {20062, 10}, {20063, 10}, {40067, 10}, {40068, 10}, }}, // ���� �ӽ�		���� ������		Ȧ�� ������
		{223, { {62, 10}, {63, 10}, {20064, 10}, {20065, 10}, {40069, 10}, {40070, 10}, }}, // ���� �ӽ�		��ũ ������		��ũ ������Ʈ
		{114, { {64, 10}, {65, 10}, {20066, 10}, {20067, 10}, {40071, 10}, {40072, 10}, }}, // ��Ʈ���̾�		������ ��Ŭ��	���� ȭ��
		{124, { {66, 10}, {67, 10}, {20068, 10}, {20069, 10}, {40073, 10}, {40074, 10}, }}, // ��������			�����̾�		������ ȭ��
		{214, { {68, 10}, {69, 10}, {20070, 10}, {20071, 10}, {40075, 10}, {40076, 10}, }}, // �������		�̸�Ż			�̺� ����
		{224, { {70, 10}, {71, 10}, {20072, 10}, {20073, 10}, {40077, 10}, {40078, 10}, }}  // ��ũ�ϼ�			ī����			���� ȭ��
	};		;
	std::vector<TenviSkill> GetAwakening(BYTE job, WORD awakening);
};

#endif