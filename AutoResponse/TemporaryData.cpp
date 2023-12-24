#include"TemporaryData.h"
#include"TenviItem.h"

DWORD TenviCharacter::id_counter = 1337;
DWORD TenviAccount::inventoryCount = 1;

// new character
TenviCharacter::TenviCharacter(std::wstring nName, BYTE nJob_Mask, WORD nJob, WORD nSkin, WORD nHair, WORD nFace, WORD nCloth, WORD nGColor, BYTE nAwakening, std::map<BYTE, Item> &nEquipped, std::map<BYTE, Item> &nGEquipped) {
	id = id_counter++;
	name = nName;
	job_mask = nJob_Mask; // gender and job
	job = nJob;
	skin = nSkin;
	hair = nHair;
	face = nFace;
	cloth = nCloth;
	gcolor = nGColor;
	equipped = nEquipped;
	gequipped = nGEquipped;
	map = 2002;
	map_return = 0;
	level = 120;
	sp = 500;
	ap = 300;
	// test stat
	stat_str = 16;
	stat_dex = 18;
	stat_hp = 199;
	stat_int = 712;
	stat_mp = 158;
	maxHP = 4000;
	HP = 3000;
	maxMP = 3000;
	MP = 2000;
	x = 0.0;
	y = 0.0;
	guardian_flag = 1;
	profile = L"TenviTest";
	titles = { 8, 9, 10, 11, 12, 13, 14 };
	awakening = nAwakening;
	fly = 0;
	aboard = 18;
	titleEquipped = 9;
	direction = 0;
	money = 12345678;
	equipLoc = 0, extraLoc = 0, questLoc = 0, cashLoc = 0, cardLoc = 0;
	InitItem();
}

void TenviCharacter::InitItem() {
	std::vector<WORD> inventory = {
		// equip
//		0x667F, 0x0745, 0x0A33, 0x01CA, 0x6837, 0x6C56, 0x7112, 0x7116, 0x6F89, 0x57E1, 0x02D6, 0x5DB0, 0x0A1B,
		655, 657, 659, 228, 230, 232, 20355, 20361, 20367, 20000, 20001, 20002, 238, 392, 391, 20357, 20363, 20369, 22000, 22001, 22002, 20356, 20362, 20368, 22500, 22505, 22508, 23001, 23000, 23029, 22495, 23432, 23442, 234, 235, 418,
		// extra
		0xEE48, 0xF677, 0xEA60, 0xEA65, 0xEA85, 0xA0DC, 40223, 0xA0E8, 0xEAF2, 0xA115, 0xA141, 0x9CDB, 0xA18B, 0xF416, 0x9D20, 0xA6C7, 0xA13B, 0xEA94, 0xEA82, 0xEB15, 0x9C40, 0x9CB8, 0x9C95, 0xA8B9,
		// quest
		42107, 0xA6B4, 0xA4DB,
		// cash
		0xF6BB, 0xA928, 0x0013, 0x000F, 0x002D, 0x0A13, 0xF64B, 0x0031, 0x06AB, 0x0099, 0x064F, 0x0036, 0x0041, 0x00A5, 0xF669, 20357, 63321, 25862, 0x6517, 26497, 26498,
//		2493, 63163, 40, 19
		// card
		30000, 30001
	};
	for (auto& itemID : inventory) {
		switch (FindType(itemID)) {
		case 0:
			inventory_equip[equipLoc++] = TenviAccount::MakeItem(itemID);
			break;
		case 1:
			inventory_extra[extraLoc++] = TenviAccount::MakeItem(itemID);
			break;
		case 2:
			inventory_quest[questLoc++] = TenviAccount::MakeItem(itemID);
			break;
		case 3:
			inventory_cash[cashLoc++] = TenviAccount::MakeItem(itemID);
			break;
		case 4:
			inventory_card[cardLoc++] = TenviAccount::MakeItem(itemID);
		}
	}
}

void TenviCharacter::SetMapReturn(WORD map_return_id) {
	map_return = map_return_id;
}

void TenviCharacter::TestSilva() {
	gcolor = 187;
	//hair = 137;
	map = 3001;
}

// game related
bool TenviCharacter::UseSP(WORD skill_id) {
	if (0 < sp) {
		sp--;
		for (auto &v : skill) {
			if (v.id == skill_id) {
				v.level++;
				return true;
			}
		}

		TenviSkill learn_skill;
		learn_skill.id = skill_id;
		learn_skill.level = 1;
		skill.push_back(learn_skill);
		return true;
	}
	return false;
}

bool TenviCharacter::UseAP(BYTE stat_id, BYTE amount) {
	if (amount <= ap) {
		ap-=amount;
		switch (stat_id) {
		case TS_STR: {
			stat_str+=amount;
			return true;
		}
		case TS_DEX: {
			stat_dex+=amount;
			return true;
		}
		case TS_HP: {
			stat_hp+=amount;
			return true;
		}
		case TS_INT: {
			stat_int+=amount;
			return true;
		}
		case TS_MP: {
			stat_mp+=amount;
			return true;
		}
		default:
		{
				break;
		}
		}
	}
	return false;
}

std::map<BYTE, Item> easyEquip(std::vector<WORD> itemVec) {
	std::map<BYTE, Item> equips;
	for (auto& itemID : itemVec) {
		equips[FindSlot(itemID)] = TenviAccount::MakeItem(itemID);
	}
	return equips;
}

// init
TenviAccount::TenviAccount() {
	slot = 6;
}

// lateinit
void TenviAccount::LateInit() {
	// default characters
	std::map<BYTE, Item> silva_gequip = easyEquip({ 270, 20502, 20002, 22319, 22848 });
	std::map<BYTE, Item> silva_equip = easyEquip({0x6501});
//	        BYTE nJob_Mask, WORD nJob, WORD nSkin, WORD nHair, WORD nFace, WORD nCloth, WORD nGColor
	TenviCharacter silva(L"Silva", 0x24, 6, 3, 15, 24, 479, 157, 124, silva_equip, silva_gequip);
	std::vector<TenviSkill> silva_basic = { {1, 1}, {50003, 1}, {40019, 1}, {50000, 1}, {50006, 1}, {50008, 5},
		{40035, 10}, {40036, 10},
		{40041, 10}, {40042, 10},
		{40053, 3}, {40054, 10},
		{40029, 10}, {40031, 10},
		{40073, 10}, {40074, 10}
	};
	silva.skill.insert(std::end(silva.skill), std::begin(silva_basic), std::end(silva_basic));
	silva.TestSilva(); // test

	std::map<BYTE, Item> talli_gequip = easyEquip({ 20811, 20001, 22411, 23968});
	std::map<BYTE, Item> talli_equip = easyEquip({0x6517, 0xF64B});
	TenviCharacter talli(L"Talli", 0x22, 5, 2, 18, 25, 476, 155, 114, talli_equip, talli_gequip);
	std::vector<TenviSkill> talli_basic = { { 1, 1 }, {30004, 1}, {20000, 1 }, {30001, 1}, {30007, 1}, {30010, 5},
		{20035, 10}, {20036, 10},
		{20039, 10}, {20040, 10},
		{20047, 3}, {20048, 10},
		{20024, 10}, {20029, 10},
		{20066, 10}, {20067, 10}
	};
	talli.skill.insert(std::end(talli.skill), std::begin(talli_basic), std::end(talli_basic));

	std::map<BYTE, Item> andras_gequip = easyEquip({ 20500, 20310, 22350, 22500, 25576});
	std::map<BYTE, Item> andras_equip = easyEquip({0x5E29, 0x6026, 0x63B4});
	TenviCharacter andras(L"Andras", 0x11, 4, 1, 17, 23, 473, 8, 214, andras_equip, andras_gequip);
	std::vector<TenviSkill> andras_basic = { { 1, 1 }, {10004, 1}, { 2, 1 }, {10001, 1}, {10007, 1}, {10010, 5},
		{33, 10}, {34, 10},
		{39, 10}, {40, 10},
		{46, 3}, {47, 10},
		{60, 10}, {61, 10},
		{68, 10}, {69, 10}
	};
	andras.skill.insert(std::end(andras.skill), std::begin(andras_basic), std::end(andras_basic));

	characters.push_back(silva);
	characters.push_back(talli);
	characters.push_back(andras);
}


bool TenviAccount::AddCharacter(std::wstring nName, BYTE nJob_Mask, WORD nJob, WORD nSkin, WORD nHair, WORD nFace, WORD nCloth, WORD nGColor, BYTE nAwakening, std::map<BYTE, Item> &nEquipped, std::map<BYTE, Item> &nGEquipped) {
	if (slot <= GetCharacters().size()) {
		return false;
	}

	TenviCharacter character(nName, nJob_Mask, nJob, nSkin, nHair, nFace, nCloth, nGColor, nAwakening, nEquipped, nGEquipped);
	characters.push_back(character);
	return true;
}

bool TenviAccount::FindCharacter(DWORD id, TenviCharacter *found) {
	for (auto &character : characters) {
		if (character.id == id) {
			*found = character;
			return true;
		}
	}

	found = NULL;
	return false;
}

std::vector<TenviCharacter>& TenviAccount::GetCharacters() {
	return characters;
}

bool TenviAccount::Login(DWORD id) {
	online_id = id;
	return true;
}

Item TenviAccount::MakeItem(WORD itemID, WORD number) {
	if (itemID == 0) {
		return Item{ 0, 0, 0, 0, 0, 0, 0};
	}
	return Item{ itemID, FindSlot(itemID), FindIsCash(itemID), FindType(itemID), FindPrice(itemID), number, inventoryCount++ };
}

TenviCharacter& TenviAccount::GetOnline() {
	for (auto &character : characters) {
		if (character.id == online_id) {
			return character;
		}
	}

	return characters[0];
}