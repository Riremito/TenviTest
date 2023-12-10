#include"TemporaryData.h"
#include"TenviItem.h"

DWORD TenviCharacter::id_counter = 1337;
DWORD TenviAccount::inventoryCount = 1;

// new character
TenviCharacter::TenviCharacter(std::wstring nName, BYTE nJob_Mask, WORD nJob, WORD nSkin, WORD nHair, WORD nFace, WORD nCloth, WORD nGColor, std::map<BYTE, Item> &nGEquipped) {
	id = id_counter++;
	name = nName;
	job_mask = nJob_Mask; // gender and job
	job = nJob;
	skin = nSkin;
	hair = nHair;
	face = nFace;
	cloth = nCloth;
	gcolor = nGColor;
	equipped.resize(15);
	gequipped = nGEquipped;
	map = 2002;
	map_return = 0;
	level = 150;
	sp = 500;
	ap = 300;
	// test stat
	stat_str = 16;
	stat_dex = 18;
	stat_hp = 199;
	stat_int = 712;
	stat_mp = 158;
	x = 0.0;
	y = 0.0;
	guardian_flag = 1;
	profile = L"TenviTest";

	InitItem();
}

void TenviCharacter::InitItem() {
	BYTE loc = 0;
	std::vector<WORD> equipInventory = {655, 657, 659, 228, 230, 232, 20355, 20361, 20367, 20000, 20001, 20002, 238, 392, 391, 20357, 20363, 20369, 22000, 22001, 22002, 20356, 20362, 20368, 22500, 22505, 22508, 23001, 23000, 23029, 22495, 23432, 23442, 234, 235, 418};
	for (auto& itemID : equipInventory) {
		inventory[loc++] = TenviAccount::MakeItem(itemID);
	}
}

void TenviCharacter::SetMapReturn(WORD map_return_id) {
	map_return = map_return_id;
}

void TenviCharacter::TestSilva() {
	gcolor = 187;
	//hair = 137;
	map = 8003;
	equipped.resize(15);
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
		equips[FindType(itemID)] = TenviAccount::MakeItem(itemID);
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
	std::map<BYTE, Item> silva_equip = easyEquip({ 270, 20502, 20002, 22319, 22848 });
	TenviCharacter silva(L"Silva", (1 << 4) | 4, 6, 3, 19, 24, 479, 157, silva_equip);
	std::vector<TenviSkill> silva_basic = { {1, 1}, {50003, 1}, {40019, 1}, {50000, 1} };
	silva.skill.insert(std::end(silva.skill), std::begin(silva_basic), std::end(silva_basic));
	silva.TestSilva(); // test

	std::map<BYTE, Item> talli_equip = easyEquip({ 20811, 20001, 22411, 23968 });
	TenviCharacter talli(L"Talli", (1 << 4) | 2, 5, 2, 18, 25, 476, 155, talli_equip);
	std::vector<TenviSkill> talli_basic = { { 1, 1 }, {30004, 1}, {20000, 1 }, {30001, 1} };
	talli.skill.insert(std::end(talli.skill), std::begin(talli_basic), std::end(talli_basic));

	std::map<BYTE, Item> andras_equip = easyEquip({ 20500, 20310, 22350, 22500 });
	TenviCharacter andras(L"Andras", (1 << 4) | 1, 4, 1, 17, 23, 473, 8, andras_equip);
	std::vector<TenviSkill> andras_basic = {{ 1, 1 }, {10004, 1}, { 2, 1 }, {10001, 1} };
	andras.skill.insert(std::end(andras.skill), std::begin(andras_basic), std::end(andras_basic));

	characters.push_back(silva);
	characters.push_back(talli);
	characters.push_back(andras);
}


bool TenviAccount::AddCharacter(std::wstring nName, BYTE nJob_Mask, WORD nJob, WORD nSkin, WORD nHair, WORD nFace, WORD nCloth, WORD nGColor, std::map<BYTE, Item> &nGEquipped) {
	if (slot <= GetCharacters().size()) {
		return false;
	}

	TenviCharacter character(nName, nJob_Mask, nJob, nSkin, nHair, nFace, nCloth, nGColor, nGEquipped);
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

Item TenviAccount::MakeItem(WORD itemID) {
	if (itemID == 0) {
		return Item{ 0, 0, 0, 0};
	}
	return Item{ itemID, FindType(itemID), FindIsCash(itemID), inventoryCount++ };
}

TenviCharacter& TenviAccount::GetOnline() {
	for (auto &character : characters) {
		if (character.id == online_id) {
			return character;
		}
	}

	return characters[0];
}