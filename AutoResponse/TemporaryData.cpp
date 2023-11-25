#include"TemporaryData.h"

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
	inventory[0] = TenviAccount::MakeItem(218, cp);
	inventory[1] = TenviAccount::MakeItem(21969, lp);
	inventory[2] = TenviAccount::MakeItem(219, cp);
	inventory[3] = TenviAccount::MakeItem(23556, th);
	inventory[4] = TenviAccount::MakeItem(23566, th);
	inventory[5] = TenviAccount::MakeItem(23831, th);
	inventory[6] = TenviAccount::MakeItem(23919, th);
	inventory[7] = TenviAccount::MakeItem(23971, th);
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
	silva_equip[cl] = MakeItem(0, cl);
	silva_equip[cp] = MakeItem(270, cp);
	silva_equip[nc] = MakeItem(0, nc);
	silva_equip[ri] = MakeItem(0, ri);
	silva_equip[ri + 1] = MakeItem(0, ri + 1);
	silva_equip[5] = MakeItem(0, 5);
	silva_equip[am] = MakeItem(20502, am);
	silva_equip[_do] = MakeItem(20002, _do);
	silva_equip[wp] = MakeItem(0, wp);
	silva_equip[lp] = MakeItem(0, lp);
	silva_equip[pp] = MakeItem(22319, pp);
	silva_equip[op] = MakeItem(0, op);
	silva_equip[dc] = MakeItem(0, dc);
	silva_equip[rh] = MakeItem(22848, rh);
	silva_equip[lh] = MakeItem(0, lh);

	TenviCharacter silva(L"Silva", (1 << 4) | 4, 6, 3, 19, 24, 479, 157, silva_equip);
	silva.TestSilva(); // test

	std::map<BYTE, Item> talli_equip;
	talli_equip[cl] = MakeItem(0, cl);
	talli_equip[cp] = MakeItem(0, cp);
	talli_equip[nc] = MakeItem(0, nc);
	talli_equip[ri] = MakeItem(0, ri);
	talli_equip[ri + 1] = MakeItem(0, ri + 1);
	talli_equip[5] = MakeItem(0, 5);
	talli_equip[am] = MakeItem(20811, am);
	talli_equip[_do] = MakeItem(20001, _do);
	talli_equip[wp] = MakeItem(0, wp);
	talli_equip[lp] = MakeItem(0, lp);
	talli_equip[pp] = MakeItem(22411, pp);
	talli_equip[op] = MakeItem(0, op);
	talli_equip[dc] = MakeItem(0, dc);
	talli_equip[rh] = MakeItem(23968, th);
	talli_equip[lh] = MakeItem(0, lh);

	TenviCharacter talli(L"Talli", (1 << 4) | 2, 5, 2, 18, 25, 476, 155, talli_equip);


	std::map<BYTE, Item> andras_equip;
	andras_equip[cl] = MakeItem(0, cl);
	andras_equip[cp] = MakeItem(0, cp);
	andras_equip[nc] = MakeItem(0, nc);
	andras_equip[ri] = MakeItem(0, ri);
	andras_equip[ri + 1] = MakeItem(0, ri + 1);
	andras_equip[5] = MakeItem(0, 5);
	andras_equip[am] = MakeItem(20500, am);
	andras_equip[_do] = MakeItem(20310, _do);
	andras_equip[wp] = MakeItem(0, wp);
	andras_equip[lp] = MakeItem(0, lp);
	andras_equip[pp] = MakeItem(22350, pp);
	andras_equip[op] = MakeItem(0, op);
	andras_equip[dc] = MakeItem(0, dc);
	andras_equip[rh] = MakeItem(22500, rh);
	andras_equip[lh] = MakeItem(0, lh);

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

Item TenviAccount::MakeItem(WORD itemID, BYTE type) {
	if (itemID == 0) {
		return Item{ 0, type, 0 };
	}
	return Item{ itemID, type, inventoryCount++ };
}

TenviCharacter& TenviAccount::GetOnline() {
	for (auto &character : characters) {
		if (character.id == online_id) {
			return character;
		}
	}

	return characters[0];
}