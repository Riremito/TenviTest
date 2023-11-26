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

	InitItem();
}
void TenviCharacter::InitItem() {
	inventory[0] = TenviAccount::MakeItem(218);
	inventory[1] = TenviAccount::MakeItem(21969);
	inventory[2] = TenviAccount::MakeItem(219);
	inventory[3] = TenviAccount::MakeItem(23556);
	inventory[4] = TenviAccount::MakeItem(23566);
	inventory[5] = TenviAccount::MakeItem(23831);
	inventory[6] = TenviAccount::MakeItem(23919);
	inventory[7] = TenviAccount::MakeItem(23971);
	// add more
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

// init
TenviAccount::TenviAccount() {
	// account info
	slot = 6;

	// default characters
	std::map<BYTE, Item> silva_equip; // itemID, type, inventoryID
	silva_equip[cl] = MakeItem(0);
	silva_equip[cp] = MakeItem(270);
	silva_equip[nc] = MakeItem(0);
	silva_equip[ri] = MakeItem(0);
	silva_equip[ri + 1] = MakeItem(0);
	silva_equip[5] = MakeItem(0);
	silva_equip[am] = MakeItem(20502);
	silva_equip[_do] = MakeItem(20002);
	silva_equip[wp] = MakeItem(0);
	silva_equip[lp] = MakeItem(0);
	silva_equip[pp] = MakeItem(22319);
	silva_equip[op] = MakeItem(0);
	silva_equip[dc] = MakeItem(0);
	silva_equip[rh] = MakeItem(22848);
	silva_equip[lh] = MakeItem(0);

	TenviCharacter silva(L"Silva", (1 << 4) | 4, 6, 3, 19, 24, 479, 157, silva_equip);
	silva.TestSilva(); // test

	std::map<BYTE, Item> talli_equip;
	talli_equip[cl] = MakeItem(0);
	talli_equip[cp] = MakeItem(0);
	talli_equip[nc] = MakeItem(0);
	talli_equip[ri] = MakeItem(0);
	talli_equip[ri + 1] = MakeItem(0);
	talli_equip[5] = MakeItem(0);
	talli_equip[am] = MakeItem(20811);
	talli_equip[_do] = MakeItem(20001);
	talli_equip[wp] = MakeItem(0);
	talli_equip[lp] = MakeItem(0);
	talli_equip[pp] = MakeItem(22411);
	talli_equip[op] = MakeItem(0);
	talli_equip[dc] = MakeItem(0);
	talli_equip[rh] = MakeItem(23968);
	talli_equip[lh] = MakeItem(0);

	TenviCharacter talli(L"Talli", (1 << 4) | 2, 5, 2, 18, 25, 476, 155, talli_equip);


	std::map<BYTE, Item> andras_equip;
	andras_equip[cl] = MakeItem(0);
	andras_equip[cp] = MakeItem(0);
	andras_equip[nc] = MakeItem(0);
	andras_equip[ri] = MakeItem(0);
	andras_equip[ri + 1] = MakeItem(0);
	andras_equip[5] = MakeItem(0);
	andras_equip[am] = MakeItem(20500);
	andras_equip[_do] = MakeItem(20310);
	andras_equip[wp] = MakeItem(0);
	andras_equip[lp] = MakeItem(0);
	andras_equip[pp] = MakeItem(22350);
	andras_equip[op] = MakeItem(0);
	andras_equip[dc] = MakeItem(0);
	andras_equip[rh] = MakeItem(22500);
	andras_equip[lh] = MakeItem(0);

	TenviCharacter andras(L"Andras", (1 << 4) | 1, 4, 1, 17, 23, 473, 8, andras_equip);

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