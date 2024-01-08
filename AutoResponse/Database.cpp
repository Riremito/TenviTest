#include"Database.h"
#include"TenviItem.h"
#include"TenviData.h"
#include"Base64.h"
#include<string>
#include<sstream>
#include<algorithm>

DWORD TenviCharacter::id_counter = 1337;
DWORD TenviAccount::inventoryCount = 1;
MYSQL* conn;

// new character
TenviCharacter::TenviCharacter(std::wstring name, std::wstring profile, DWORD id, BYTE job_mask, WORD job, WORD skin,
	WORD hair, WORD face, WORD gcolor, BYTE awakening, WORD map, BYTE level, WORD sp, WORD ap,
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
	direction = 1;
	isLoaded = false;
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

Item TenviCharacter::GetItemByItemID(WORD itemID) {
	char query[1024];
	MYSQL_RES* result;
	sprintf_s(query, 1024, "SELECT * FROM tables.inventory WHERE chr_id = %d AND itemID = %d ORDER BY loc ASC", id, itemID);
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
	if (pre + 1 < 40) {
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

		MYSQL_RES* skillFound;
		sprintf_s(query, 1024, "SELECT level FROM tables.skill WHERE chr_id = %d AND skill_id = %d", id, skill_id);
		mysql_query(conn, query);
		skillFound = mysql_store_result(conn);
		MYSQL_ROW s;

		if (s = mysql_fetch_row(skillFound)) {
			sprintf_s(query, 1024, "UPDATE tables.skill SET level = level + 1 WHERE chr_id = %d AND skill_id = %d", id, skill_id);
			mysql_query(conn, query);
		}
		else {
			sprintf_s(query, 1024, "INSERT INTO tables.skill VALUES (%d, %d, %d)", id, skill_id, 1);
			mysql_query(conn, query);
		}
		mysql_free_result(skillFound);
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

std::vector<TenviSkill> TenviCharacter::GetSkill() {
	char query[1024];
	MYSQL_RES* skill_list;
	sprintf_s(query, 1024, "SELECT skill_id, level FROM tables.skill WHERE chr_id = %d", id);
	mysql_query(conn, query);
	skill_list = mysql_store_result(conn);
	MYSQL_ROW skill;
	std::vector<TenviSkill> res_skill;
	while (skill = mysql_fetch_row(skill_list)) {
		TenviSkill s;
		s.id = atoi(skill[0]);
		s.level = atoi(skill[1]);
		res_skill.push_back(s);
	}
	for (TenviSkill& skill : TenviAccount::GetAwakening(job, awakening)) {
		res_skill.push_back(skill);
	}
	mysql_free_result(skill_list);
	return res_skill;
}

void TenviCharacter::SetHP(WORD hp) {
	char query[1024];
	HP = hp;
	sprintf_s(query, 1024, "UPDATE tables.character set HP = %d WHERE id = %d", HP, id);
	mysql_query(conn, query);
}

void TenviCharacter::SetMP(WORD mp) {
	char query[1024];
	MP = mp;
	sprintf_s(query, 1024, "UPDATE tables.character set MP = %d WHERE id = %d", MP, id);
	mysql_query(conn, query);
}

void TenviCharacter::HealHP(WORD amount) {
	char query[1024];
	HP = HP + amount > maxHP ? maxHP : HP + amount;
	sprintf_s(query, 1024, "UPDATE tables.character set HP = %d WHERE id = %d", HP, id);
	mysql_query(conn, query);
}

void TenviCharacter::HealMP(WORD amount) {
	char query[1024];
	MP = MP + amount > maxMP ? maxMP : MP + amount;
	sprintf_s(query, 1024, "UPDATE tables.character set MP = %d WHERE id = %d", MP, id);
	mysql_query(conn, query);
}

void TenviCharacter::KeySet(std::string str) {
	char query[1024];
	const char* keys = str.c_str();
	sprintf_s(query, 1024, "UPDATE tables.character set keySet = \"%s\" WHERE id = %d", keys, id);
	mysql_query(conn, query);
}

std::vector<BYTE> TenviCharacter::GetKeySet() {
	char query[1024];
	MYSQL_RES* result;
	std::vector<BYTE> res;
	sprintf_s(query, 1024, "SELECT keySet FROM tables.character WHERE id = %d AND keySet", id);
	mysql_query(conn, query);
	result = mysql_store_result(conn);
	MYSQL_ROW keys = mysql_fetch_row(result);
	if (!keys || !keys[0] || keys[0] == "0" || strcmp(keys[0], "") == 0) {
		res.push_back(0);
		res.push_back(0);
		mysql_free_result(result);
		return res;
	}
	std::string key_string = std::string(keys[0]);

	std::istringstream ss(key_string);
	std::string stringBuffer;
	while (getline(ss, stringBuffer, '/')) {
		res.push_back(std::stoi(stringBuffer));
	}
	mysql_free_result(result);
	return res;
}

void TenviCharacter::ChangeTitle(BYTE code) {
	char query[1024];
	titleEquipped = code;
	sprintf_s(query, 1024, "UPDATE tables.character set titleEquipped = %d WHERE id = %d", titleEquipped, id);
	mysql_query(conn, query);
}

void TenviCharacter::ChangeMoney(DWORD money) {
	char query[1024];
	this->money = money;
	sprintf_s(query, 1024, "UPDATE tables.character set money = %d WHERE id = %d", money, id);
	mysql_query(conn, query);
}

void TenviCharacter::ChangeProfile(std::wstring wText) {
	profile = wText;
	char query[1024];
	sprintf_s(query, 1024, "UPDATE tables.character set profile = \"%s\" WHERE id = %d", base64_encode(WstrToStr(wText), true).c_str(), id);
	mysql_query(conn, query);
}

void TenviCharacter::SortInventory(BYTE method, BYTE type) {
	// type: 0, 1, 2
	// 0: loc, 1: group, 3: rank
	char query[4096];
	MYSQL_RES* result;
	switch (method) {
	case 0: {
		sprintf_s(query, 4096, "SELECT inventoryID FROM tables.inventory WHERE chr_id = %d AND type = %d AND isEquip = 0 ORDER BY loc", id, type);
		break;
	}
	case 1: {
		sprintf_s(query, 4096, "SELECT inventoryID FROM tables.inventory WHERE chr_id = %d AND type = %d AND isEquip = 0 ORDER BY _group", id, type);
		break;
	}
	case 2: {
		sprintf_s(query, 4096, "SELECT inventoryID FROM tables.inventory WHERE chr_id = %d AND type = %d AND isEquip = 0 ORDER BY rank", id, type);
		break;
	}
	}
	mysql_query(conn, query);
	result = mysql_store_result(conn);

	for (int newLoc = 0; newLoc < mysql_num_rows(result); newLoc++) {
		MYSQL_ROW item = mysql_fetch_row(result);
		DWORD inventoryID = atoi(item[0]);
		writeDebugLog(std::to_string(inventoryID));
		sprintf_s(query, 1024, "UPDATE tables.inventory SET loc = %d WHERE chr_id = %d AND inventoryID = %d", newLoc, id, inventoryID);
		mysql_query(conn, query);
	}
	mysql_free_result(result);
}

std::vector<TenviSkill> TenviAccount::GetAwakening(BYTE job, WORD awakening) {
	// awakening skill map
	// 100 => 111 => 112 => 113 => 114
	// 100 => 121 => 122 => 123 => 124
	// 200 => 211 => 212 => 213 => 214
	// 200 => 221 => 222 => 223 => 224

	std::vector<TenviSkill> awakeningSkill;
	auto pushSkill = [&](WORD awakening) {
		awakeningSkill.push_back(AwakeningMap[awakening][(job - 4) * 2]);
		awakeningSkill.push_back(AwakeningMap[awakening][(job - 4) * 2 + 1]);
		};

	switch (awakening) {
	case 0:
		return awakeningSkill;
	case 114:
		pushSkill(114);
	case 113:
		pushSkill(113);
	case 112:
		pushSkill(112);
	case 111:
		pushSkill(111);
	case 100:
		pushSkill(100);
		break;
	case 124:
		pushSkill(124);
	case 123:
		pushSkill(123);
	case 122:
		pushSkill(122);
	case 121:
		pushSkill(121);
		pushSkill(100);
		break;
	case 214:
		pushSkill(214);
	case 213:
		pushSkill(213);
	case 212:
		pushSkill(212);
	case 211:
		pushSkill(211);
	case 200:
		pushSkill(200);
		break;
	case 224:
		pushSkill(224);
	case 223:
		pushSkill(223);
	case 222:
		pushSkill(222);
	case 221:
		pushSkill(221);
		pushSkill(200);
		break;
	}
	std::reverse(awakeningSkill.begin(), awakeningSkill.end());
	return awakeningSkill;
}

// init
DWORD TenviAccount::objectCounter = 1;

TenviAccount::TenviAccount() {
	unsigned int timeout_sec = 1;
	objectCounter = 1;

	conn = mysql_init(NULL);
	mysql_options(conn, MYSQL_OPT_CONNECT_TIMEOUT, &timeout_sec);
	try {
		mysql_real_connect(conn, "127.0.0.1", "root", "1234", "tables", 3306, NULL, 0);
	}
	catch (...) {
		writeDebugLog(mysql_error(conn));
	}

	char query[1024];
	MYSQL_RES* result;
	sprintf_s(query, 1024, "SELECT * FROM tables.character ORDER BY no ASC");
	mysql_query(conn, query);
	result = mysql_store_result(conn);

	int fields = mysql_num_fields(result);
	objectCounter = 1;

	std::map<std::string, BYTE> getf;
	MYSQL_FIELD* field;
	for (unsigned int i = 0; (field = mysql_fetch_field(result)); i++) {
		getf[field->name] = i;
	}

	MYSQL_ROW chr;
	while (chr = mysql_fetch_row(result)) {
		auto getInt = [&](std::string field_name) { return atoi(chr[getf[field_name]]); };
		std::wstring name = StrToWstr(base64_decode(chr[getf["name"]], true));
		std::wstring profile = StrToWstr(base64_decode(chr[getf["profile"]], true));
		DWORD id = getInt("id");
		BYTE job_mask = getInt("job_mask");
		WORD job = getInt("job");
		WORD skin = getInt("skin");
		WORD hair = getInt("hair");
		WORD face = getInt("face");
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

		TenviCharacter player(name, profile, id, job_mask, job, skin, hair, face, gcolor, awakening,
			map, level, sp, ap, stat_str, stat_dex, stat_hp, stat_int, stat_mp, maxHP, HP, maxMP, MP,
			titleEquipped, money);

		characters.push_back(player);
	}
	slot = characters.size();
	mysql_free_result(result);
}

bool TenviAccount::AddCharacter(std::wstring nName, BYTE nJob_Mask, WORD nJob, WORD nSkin, WORD nHair, WORD nFace, WORD nCloth,
	WORD nGColor, WORD gHead, WORD gBody, WORD gWeapon) {
	if (slot == 6) {
		return false;
	}

	DWORD highestID = 1;
	for (TenviCharacter& _chr : characters) {
		if (_chr.id > highestID) {
			highestID = _chr.id;
		}
	}

	TenviCharacter character(nName, L"", highestID + 1, nJob_Mask, nJob, nSkin, nHair, nFace, nGColor,
		0, 5501, 1, 1000, 20, 20, 20, 20, 20, 20, 1000, 1000, 800, 800, 0, 12345678);
	characters.push_back(character);
	char query[4096];
	MYSQL_RES* skill_list;

	std::string _name = base64_encode(WstrToStr(nName), true);
	DWORD id = highestID + 1;
	sprintf_s(query, 4096, "INSERT INTO tables.character (no, id, name, job_mask, job, skin, \
hair, face, gcolor, awakening, map, level, sp, ap, stat_str, stat_dex, stat_hp, stat_int, \
stat_mp, maxHP, HP, maxMP, MP, titleEquipped, money, profile, keySet) VALUES \
(%d, %d, \"%s\", %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, \"%s\", \"%s\")",
++slot, id, _name.c_str(), nJob_Mask, nJob, nSkin, nHair, nFace, nGColor,
0, 5501, 1, 1000, 20, 10, 10, 10, 10, 10, 1000, 800, 900, 800, 0, 12345678, "", "");
	mysql_query(conn, query);

	character.AddItem(MakeItem(character, gHead, 1));
	character.AddItem(MakeItem(character, gBody, 1));
	character.AddItem(MakeItem(character, gWeapon, 1));
	character.AddItem(MakeItem(character, nCloth, 1));
	character.AddItem(MakeItem(character, 63163, 0)); // telescope

	sprintf_s(query, 1024, "UPDATE tables.inventory SET isEquip = 1, loc = 0 WHERE chr_id = %d AND itemID = %d", character.id, gHead);
	mysql_query(conn, query);
	sprintf_s(query, 1024, "UPDATE tables.inventory SET isEquip = 1, loc = 0 WHERE chr_id = %d AND itemID = %d", character.id, gBody);
	mysql_query(conn, query);
	sprintf_s(query, 1024, "UPDATE tables.inventory SET isEquip = 1, loc = 0 WHERE chr_id = %d AND itemID = %d", character.id, gWeapon);
	mysql_query(conn, query);
	sprintf_s(query, 1024, "UPDATE tables.inventory SET isEquip = 1, loc = 0 WHERE chr_id = %d AND itemID = %d", character.id, nCloth);
	mysql_query(conn, query);

	std::map<BYTE, std::vector<TenviSkill>> basicSkill;
	basicSkill[4] = { {1, 1}, {2, 1}, {10001, 1}, {10004, 1} };
	basicSkill[5] = { {1, 1}, {20000, 1}, {30001, 1}, {30004, 1} };
	basicSkill[6] = { {1, 1}, {40019, 1}, {50000, 1}, {50003, 1} };

	for(TenviSkill& skill : basicSkill[nJob]) {
		sprintf_s(query, 1024, "INSERT INTO tables.skill VALUES (%d, %d, %d)", id, skill.id, skill.level);
		mysql_query(conn, query);
	}

	return true;
}

bool TenviAccount::DeleteCharacter(DWORD chr_id) {
	char query[1024];
	bool isFound = false;
	for (auto it = characters.begin(); it != characters.end(); it++) {
		if (it->id == chr_id) {
			sprintf_s(query, 1024, "DELETE FROM tables.character WHERE id = %d", it->id);
			mysql_query(conn, query);
			sprintf_s(query, 1024, "DELETE FROM tables.inventory WHERE chr_id = %d", it->id);
			mysql_query(conn, query);
			sprintf_s(query, 1024, "DELETE FROM tables.skill WHERE chr_id = %d", it->id);
			mysql_query(conn, query);
			characters.erase(it);
			isFound = true;
			break;
		}
	}
	if (isFound) {
		slot = characters.size();
		for (int i = 0; i < characters.size(); i++) {
			sprintf_s(query, 1024, "UPDATE tables.character SET no = %d WHERE id = %d", i+1, characters[i].id);
			mysql_query(conn, query);
		}
		return true;
	}
	return false;
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

	MYSQL_ROW item;
	if (item = mysql_fetch_row(result)) {
		mysql_free_result(result);
		return atoi(item[0]);
	}
	mysql_free_result(result);
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

DWORD TenviAccount::GetObjectID() {
	return objectCounter++;
}

void TenviAccount::ClearObjectID() {
	objectCounter = 1;
}


TenviCharacter& TenviAccount::GetOnline() {
	for (auto &character : characters) {
		if (character.id == online_id) {
			return character;
		}
	}
	return characters[0];
}

std::map<WORD, std::vector<TenviSkill>> TenviAccount::AwakeningMap = {
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
};