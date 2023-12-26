#include"Database.h"
#include"TenviItem.h"
#include <string>
#include"TenviData.h"

DWORD TenviCharacter::id_counter = 1337;
DWORD TenviAccount::inventoryCount = 1;
MYSQL* conn;

// new character
TenviCharacter::TenviCharacter(std::wstring name, std::wstring profile, DWORD id, BYTE job_mask, WORD job, WORD skin,
	WORD hair, WORD face, WORD cloth, WORD gcolor, BYTE awakening, WORD map, BYTE level, WORD sp, WORD ap,
	WORD stat_str, WORD stat_dex, WORD stat_hp, WORD stat_int, WORD stat_mp, WORD maxHP, WORD HP, WORD maxMP,
	WORD MP, BYTE titleEquipped, DWORD money) {

	this->name = name;
	this->profile = profile;
	this->id = id;
	this->job_mask = job_mask;
	this->job = job;
	this->skin = skin;
	this->hair = hair;
	this->face = face;
	this->cloth = cloth;
	this->gcolor = gcolor;
	this->awakening = awakening;
	this->map = map;
	this->level = level;
	this->sp = sp;
	this->ap = ap;
	this->stat_str = stat_str;
	this->stat_dex = stat_dex;
	this->stat_hp = stat_hp;
	this->stat_int = stat_int;
	this->stat_mp = stat_mp;
	this->maxHP = maxHP;
	this->HP = HP;
	this->maxMP = maxMP;
	this->MP = MP;
	this->titleEquipped = titleEquipped;
	this->money = money;

	map_return = 0;
	x = 0.0;
	y = 0.0;
	guardian_flag = 1;
	titles = { 8, 9, 10, 11, 12, 13, 14 };
	fly = 0;
	aboard = 18;
	direction = 0;
}

Item extractItemRow(MYSQL_ROW item, std::map<std::string, BYTE> getf) {
	auto getInt = [&](std::string field_name) { return atoi(item[getf[field_name]]); };
	Item myItem;
	myItem.inventoryID = atoi(item[getf["inventoryID"]]);
	myItem.itemID = atoi(item[getf["itemID"]]);
	myItem.type = atoi(item[getf["type"]]);
	myItem.group = atoi(item[getf["group"]]);
	myItem.slot = atoi(item[getf["slot"]]);
	myItem.rank = atoi(item[getf["rank"]]);
	myItem.isCash = atoi(item[getf["isCash"]]);
	myItem.price = atoi(item[getf["price"]]);
	myItem.loc = atoi(item[getf["loc"]]);
	myItem.number = atoi(item[getf["number"]]);
	myItem.isEquip = atoi(item[getf["isEquip"]]);
	return myItem;
}

std::map<BYTE, Item> TenviCharacter::GetEquipped(BYTE isCash) {
	char query[1024];
	MYSQL_RES* result;
	sprintf_s(query, 1024, "SELECT * FROM tables.inventory WHERE chr_id = %d AND isEquip = 1 AND isCash = %d", id, isCash);
	mysql_query(conn, query);
	result = mysql_store_result(conn);
	std::map<std::string, BYTE> getf;
	MYSQL_FIELD* field;
	for (unsigned int i = 0; (field = mysql_fetch_field(result)); i++) {
		getf[field->name] = i;
	}

	MYSQL_ROW item;
	std::map<BYTE, Item> equip;
	while (item = mysql_fetch_row(result)) {
		auto getInt = [&](std::string field_name) { return atoi(item[getf[field_name]]); };
		Item myItem = extractItemRow(item, getf);
		equip[myItem.slot] = myItem;
	}
	mysql_free_result(result);
	return equip;
}

std::vector<Item> TenviCharacter::GetInventory() {
	char query[1024];
	MYSQL_RES* result;
	sprintf_s(query, 1024, "SELECT * FROM tables.inventory WHERE chr_id = %d AND isEquip = 0", id);
	mysql_query(conn, query);
	result = mysql_store_result(conn);

	std::map<std::string, BYTE> getf;
	MYSQL_FIELD* field;

	for (unsigned int i = 0; (field = mysql_fetch_field(result)); i++) {
		getf[field->name] = i;
	}

	MYSQL_ROW item;
	std::vector<Item> inventory;
	while (item = mysql_fetch_row(result)) {
		inventory.push_back(extractItemRow(item, getf));
	}
	mysql_free_result(result);
	return inventory;
}

Item TenviCharacter::GetItemByLoc(BYTE type, BYTE loc) {
	char query[1024];
	MYSQL_RES* result;
	sprintf_s(query, 1024, "SELECT * FROM tables.inventory WHERE chr_id = %d AND isEquip = 0 AND type = %d AND loc = %d", id, type, loc);
	mysql_query(conn, query);
	result = mysql_store_result(conn);

	std::map<std::string, BYTE> getf;
	MYSQL_FIELD* field;
	for (unsigned int i = 0; (field = mysql_fetch_field(result)); i++) {
		getf[field->name] = i;
	}

	Item myItem = {};
	MYSQL_ROW item;
	if (item = mysql_fetch_row(result)) {
		myItem = extractItemRow(item, getf);
	}
	mysql_free_result(result);
	return myItem;
}

Item TenviCharacter::GetItemBySlot(BYTE slot, BYTE type) {
	char query[1024];
	MYSQL_RES* result;
	BYTE isCash = type == 3 ? 1 : 0;
	sprintf_s(query, 1024, "SELECT * FROM tables.inventory WHERE chr_id = %d AND isEquip = 1 AND slot = %d AND isCash = %d", id, slot, isCash);
	mysql_query(conn, query);
	result = mysql_store_result(conn);

	std::map<std::string, BYTE> getf;
	MYSQL_FIELD* field;
	for (unsigned int i = 0; (field = mysql_fetch_field(result)); i++) {
		getf[field->name] = i;
	}

	Item myItem = {};
	MYSQL_ROW item;
	if (item = mysql_fetch_row(result)) {
		myItem = extractItemRow(item, getf);
	}
	mysql_free_result(result);
	return myItem;
}

Item TenviCharacter::GetItemByInventoryID(DWORD inventoryID) {
	char query[1024];
	MYSQL_RES* result;
	sprintf_s(query, 1024, "SELECT * FROM tables.inventory WHERE chr_id = %d AND inventoryID = %d", id, inventoryID);
	mysql_query(conn, query);
	result = mysql_store_result(conn);

	std::map<std::string, BYTE> getf;
	MYSQL_FIELD* field;
	for (unsigned int i = 0; (field = mysql_fetch_field(result)); i++) {
		getf[field->name] = i;
	}

	Item myItem = {};
	MYSQL_ROW item;
	if (item = mysql_fetch_row(result)) {
		myItem = extractItemRow(item, getf);
	}
	mysql_free_result(result);
	return myItem;
}

void TenviCharacter::ChangeItemLoc(DWORD inventoryID, BYTE loc) {
	char query[1024];
	sprintf_s(query, 1024, "UPDATE tables.inventory SET loc = %d WHERE chr_id = %d AND inventoryID = %d", loc, id, inventoryID);
	mysql_query(conn, query);
}

void TenviCharacter::DeleteItem(DWORD inventoryID) {
	char query[1024];
	sprintf_s(query, 1024, "DELETE FROM tables.inventory WHERE chr_id = %d AND inventoryID = %d", id, inventoryID);
	mysql_query(conn, query);
}

void TenviCharacter::ChangeItemNumber(DWORD inventoryID, WORD number) {
	char query[1024];
	sprintf_s(query, 1024, "UPDATE tables.inventory SET number = %d WHERE chr_id = %d AND inventoryID = %d", number, id, inventoryID);
	mysql_query(conn, query);
}


void TenviCharacter::SwitchRing(BYTE isCash) {
	char query[1024];
	sprintf_s(query, 1024, "UPDATE tables.inventory SET slot = 20 WHERE chr_id = %d AND slot = 3 AND isCash = %d AND isEquip = 1", id, isCash);
	mysql_query(conn, query);
	sprintf_s(query, 1024, "UPDATE tables.inventory SET slot = 3 WHERE chr_id = %d AND slot = 4 AND isCash = %d AND isEquip = 1", id, isCash);
	mysql_query(conn, query);
	sprintf_s(query, 1024, "UPDATE tables.inventory SET slot = 4 WHERE chr_id = %d AND slot = 20 AND isCash = %d AND isEquip = 1", id, isCash);
	mysql_query(conn, query);
}
void TenviCharacter::EquipItem(Item item, BYTE ring4) {
	// 원래 착용 중인 장비가 있다면 isEquip = 0, loc = item.loc으로 변경
	item.slot = ring4 ? 4 : item.slot;
	Item pre = GetItemBySlot(item.slot, item.type);
	if (pre.itemID) {
		char query[1024];
		sprintf_s(query, 1024, "UPDATE tables.inventory SET isEquip = 0, loc = %d WHERE chr_id = %d AND inventoryID = %d", item.loc, id, pre.inventoryID);
		mysql_query(conn, query);
	}

	// item에 해당하는 row를 찾아서 isEquip = 1, loc = 0으로 변경
	char query[1024];
	sprintf_s(query, 1024, "UPDATE tables.inventory SET isEquip = 1, loc = 0 WHERE chr_id = %d AND inventoryID = %d", id, item.inventoryID);
	mysql_query(conn, query);

	// 오른손 반지인 경우 item의 slot을 4로 변경
	if (ring4) {
		char query[1024];
		sprintf_s(query, 1024, "UPDATE tables.inventory SET slot = 4 WHERE chr_id = %d AND inventoryID = %d", id, item.inventoryID);
		mysql_query(conn, query);
	}
}

void TenviCharacter::UnequipItem(Item item, BYTE loc, BYTE ring4) {
	// item에 해당하는 row를 찾아서 isEquip = 0, loc = loc으로 변경
	char query[1024];
	sprintf_s(query, 1024, "UPDATE tables.inventory SET isEquip = 0, loc = %d WHERE chr_id = %d AND inventoryID = %d", loc, id, item.inventoryID);
	mysql_query(conn, query);

	// 오른손 반지를 해제하는 경우 item의 slot을 3로 변경
	if (ring4) {
		char query1[1024];
		sprintf_s(query1, 1024, "UPDATE tables.inventory SET slot = 3 WHERE chr_id = %d AND inventoryID = %d", id, item.inventoryID);
		mysql_query(conn, query1);
	}
}

int TenviCharacter::GetEmptyLoc(BYTE type) {
	char query[1024];
	MYSQL_RES* result;
	sprintf_s(query, 1024, "SELECT loc FROM tables.inventory WHERE chr_id = %d AND isEquip = 0 AND type = %d ORDER BY loc ASC", id, type);
	mysql_query(conn, query);
	result = mysql_store_result(conn);

	std::map<std::string, BYTE> getf;
	MYSQL_FIELD* field;
	for (unsigned int i = 0; (field = mysql_fetch_field(result)); i++) {
		getf[field->name] = i;
	}

	MYSQL_ROW item;
	std::vector<Item> inventory;
	while (item = mysql_fetch_row(result)) {
		inventory.push_back(extractItemRow(item, getf));
	}
	mysql_free_result(result);

	int pre = -1;
	for (Item& item : inventory) {
		if (item.loc == pre + 1) {
			pre++;
			continue;
		}
		return pre + 1;
	}
	if (pre + 1 <= 40) {
		return pre + 1;
	}
	return -1;
}

void TenviCharacter::AddItem(Item item) {
	// chr_id, inventoryID, itemID, type, slot, isCash, price, number, isEquip, group, rank, loc
	char query[1024];
	MYSQL_RES* result;
	sprintf_s(query, 1024, "INSERT INTO tables.inventory VALUES (%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d)",
		id, item.inventoryID, item.itemID, item.type, item.slot, item.isCash, item.price, item.number, item.isEquip, item.group, item.rank, item.loc);
	mysql_query(conn, query);
}

void TenviCharacter::SetMapReturn(WORD map_return_id) {
	map_return = map_return_id;
}

// game related
bool TenviCharacter::UseSP(WORD skill_id) {
	char query[1024];
	if (0 < sp) {
		sp--;
		sprintf_s(query, 1024, "UPDATE tables.character SET sp = %d WHERE id = %d", sp, id);
		mysql_query(conn, query);

		for (auto &v : skill) {
			if (v.id == skill_id) {
				v.level++;
				MYSQL_RES* result;
				sprintf_s(query, 1024, "UPDATE tables.skill SET level = level + 1 WHERE chr_id = %d AND skill_id = %d", id, skill_id);
				mysql_query(conn, query);
				return true;
			}
		}
		TenviSkill learn_skill;
		learn_skill.id = skill_id;
		learn_skill.level = 1;
		skill.push_back(learn_skill);

		sprintf_s(query, 1024, "INSERT INTO tables.skill VALUES (%d, %d, %d)", id, skill_id, 1);
		mysql_query(conn, query);
		return true;
	}
	return false;
}

bool TenviCharacter::UseAP(BYTE stat_id, BYTE amount) {
	char query[1024];

	if (amount <= ap) {
		ap-=amount;
		sprintf_s(query, 1024, "UPDATE tables.character set ap = %d WHERE id = %d", ap, id);
		mysql_query(conn, query);
		switch (stat_id) {
		case TS_STR: {
			stat_str+=amount;
			sprintf_s(query, 1024, "UPDATE tables.character set stat_str = %d WHERE id = %d", stat_str, id);
			mysql_query(conn, query);
			return true;
		}
		case TS_DEX: {
			stat_dex+=amount;
			sprintf_s(query, 1024, "UPDATE tables.character set stat_dex = %d WHERE id = %d", stat_dex, id);
			mysql_query(conn, query);
			return true;
		}
		case TS_HP: {
			stat_hp+=amount;
			sprintf_s(query, 1024, "UPDATE tables.character set stat_hp = %d WHERE id = %d", stat_hp, id);
			mysql_query(conn, query);
			return true;
		}
		case TS_INT: {
			stat_int+=amount;
			sprintf_s(query, 1024, "UPDATE tables.character set stat_int= %d WHERE id = %d", stat_int, id);
			mysql_query(conn, query);
			return true;
		}
		case TS_MP: {
			stat_mp+=amount;
			sprintf_s(query, 1024, "UPDATE tables.character set stat_mp = %d WHERE id = %d", stat_mp, id);
			mysql_query(conn, query);
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

void TenviCharacter::SetMap(WORD map_id) {
	map = map_id;
	char query[1024];
	sprintf_s(query, 1024, "UPDATE tables.character set map = %d WHERE id = %d", map_id, id);
	mysql_query(conn, query);
}

void TenviCharacter::RefreshHPMP() {
	char query[1024];
	sprintf_s(query, 1024, "UPDATE tables.character set HP = %d, MP = %d WHERE id = %d", HP, MP, id);
	mysql_query(conn, query);
}

void TenviCharacter::ChangeTitle(BYTE code) {
	char query[1024];
	titleEquipped = code;
	sprintf_s(query, 1024, "UPDATE tables.character set titleEquipped = %d WHERE id = %d", titleEquipped, id);
	mysql_query(conn, query);
}

// init
TenviAccount::TenviAccount() {
	unsigned int timeout_sec = 1;
	conn = mysql_init(NULL);
	mysql_options(conn, MYSQL_OPT_CONNECT_TIMEOUT, &timeout_sec);
	mysql_real_connect(conn, "127.0.0.1", "root", "1234", "tables", 3306, NULL, 0);

	char query[1024];
	MYSQL_RES* result;
	sprintf_s(query, 1024, "SELECT * FROM tables.character ORDER BY no ASC");
	mysql_query(conn, query);
	result = mysql_store_result(conn);

	int fields = mysql_num_fields(result);
	slot = mysql_num_rows(result);

	std::map<std::string, BYTE> getf;
	MYSQL_FIELD* field;
	for (unsigned int i = 0; (field = mysql_fetch_field(result)); i++) {
		getf[field->name] = i;
	}

	MYSQL_ROW chr;
	while (chr = mysql_fetch_row(result)) {
		auto getInt = [&](std::string field_name) { return atoi(chr[getf[field_name]]); };
		std::wstring name = StrToWstr(std::string(chr[getf["name"]]));
		std::wstring profile = StrToWstr(std::string(chr[getf["profile"]]));
		DWORD id = getInt("id");
		BYTE job_mask = getInt("job_mask");
		WORD job = getInt("job");
		WORD skin = getInt("skin");
		WORD hair = getInt("hair");
		WORD face = getInt("face");
		WORD cloth = getInt("cloth");
		WORD gcolor = getInt("gcolor");
		BYTE awakening = getInt("awakening");
		WORD map = getInt("map");
		BYTE level = getInt("level");
		WORD sp = getInt("sp");
		WORD ap = getInt("ap");
		WORD stat_str = getInt("stat_str");
		WORD stat_dex = getInt("stat_dex");
		WORD stat_hp = getInt("stat_hp");
		WORD stat_int = getInt("stat_int");
		WORD stat_mp = getInt("stat_mp");
		WORD maxHP = getInt("maxHP");
		WORD HP = getInt("HP");
		WORD maxMP = getInt("maxMP");
		WORD MP = getInt("MP");
		BYTE titleEquipped = getInt("titleEquipped");
		DWORD money = getInt("money");

		TenviCharacter player(name, profile, id, job_mask, job, skin, hair, face, cloth, gcolor, awakening,
			map, level, sp, ap, stat_str, stat_dex, stat_hp, stat_int, stat_mp, maxHP, HP, maxMP, MP,
			titleEquipped, money);

		char query[1024];
		MYSQL_RES* skill_list;
		sprintf_s(query, 1024, "SELECT skill_id, level FROM tables.skill WHERE chr_id = %d", player.id);
		mysql_query(conn, query);
		skill_list = mysql_store_result(conn);
		MYSQL_ROW skill;
		while (skill = mysql_fetch_row(skill_list)) {
			TenviSkill s;
			s.id = atoi(skill[0]);
			s.level = atoi(skill[1]);
			player.skill.push_back(s);
		}
		mysql_free_result(skill_list);
		characters.push_back(player);
		writeDebugLog("here");
	}
	mysql_free_result(result);

}

bool TenviAccount::AddCharacter(std::wstring nName, BYTE nJob_Mask, WORD nJob, WORD nSkin, WORD nHair, WORD nFace, WORD nCloth, WORD nGColor, BYTE nAwakening, std::map<BYTE, Item> &nEquipped, std::map<BYTE, Item> &nGEquipped) {
	if (slot <= GetCharacters().size()) {
		return false;
	}

//	TenviCharacter character(nName, nJob_Mask, nJob, nSkin, nHair, nFace, nCloth, nGColor, nAwakening, nEquipped, nGEquipped);
//	characters.push_back(character);
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

DWORD TenviAccount::GetHighestInventoryID() {
	char query[1024];
	MYSQL_RES* result;
	sprintf_s(query, 1024, "SELECT inventoryID FROM tables.inventory ORDER BY inventoryID DESC");
	mysql_query(conn, query);
	result = mysql_store_result(conn);

	std::map<std::string, BYTE> getf;
	MYSQL_FIELD* field;
	for (unsigned int i = 0; (field = mysql_fetch_field(result)); i++) {
		getf[field->name] = i;
	}

	MYSQL_ROW item;
	if (item = mysql_fetch_row(result)) {
		return atoi(item[0]);
	}
	return 0;
}

Item TenviAccount::MakeItem(TenviCharacter& chr, WORD itemID, WORD number) {
	Item item;
	if (itemID == 0) {
		return Item{ };
	}
	item.type = FindType(itemID);
	if (chr.GetEmptyLoc(item.type) == -1) {
		return Item{ };
	}
	item.inventoryID = GetHighestInventoryID() + 1;
	item.loc = chr.GetEmptyLoc(item.type);
	item.itemID = itemID;
	item.slot = FindSlot(itemID);
	item.isCash = FindIsCash(itemID);
	item.price = FindPrice(itemID);
	item.number = number;
	item.isEquip = 0;
	item.group = FindGroup(itemID);
	item.rank = FindRank(itemID);
	return item;
}

TenviCharacter& TenviAccount::GetOnline() {
	for (auto &character : characters) {
		if (character.id == online_id) {
			return character;
		}
	}

	return characters[0];
}