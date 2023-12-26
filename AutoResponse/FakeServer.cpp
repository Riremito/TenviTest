#include"FakeServer.h"
#include"AutoResponse.h"
#include"Database.h"
#include"TenviItem.h"
#include<map>
#include<set>
#include<ctime>
#include <clocale>
#include <locale>


TenviAccount TA;
// ========== TENVI Packet Response ==========s
#define TENVI_VERSION 0x1023

// 0x01
void VersionPacket() {
	ServerPacket sp(SP_VERSION);
	sp.Encode4(TENVI_VERSION); // 00491388, version
	sp.Encode4(1); // 00491391
	sp.Encode4(1); // 0049139A
	sp.Encode4(1); // 004913A4
	SendPacket(sp);
}

// 0x02
void CrashPacket() {
	ServerPacket sp(SP_CRASH);
	sp.Encode4(TENVI_VERSION); // version
	SendPacket(sp);
}

// 0x03
void LoginFailedPacket() {
	ServerPacket sp(SP_LOGIN_FAILED);
	SendPacket(sp);
}

// 0x04
void CharacterSelectPacket() {
	ServerPacket sp(SP_CHARACTER_SELECT);
	sp.Encode1(0); // 00492CE6, 0 = OK, 1 = login error
	sp.Encode4(-1); // 00492D5B saved to pointer
	sp.Encode1(0); // 00492D65 saved to pointer

	if (GetRegion() == TENVI_KR) {
		sp.Encode1(0);
		sp.EncodeWStr1(L"Tenvi KR v200");
	}

	SendPacket(sp);
}

// 0x05
int character_slot = 6;
void CharacterListPacket() {
	ServerPacket sp(SP_CHARACTER_LIST);
	sp.Encode1(0); // Number of Characters
	sp.Encode1(character_slot); // Number of Character Slot
	DelaySendPacket(sp);
}

void CharacterListPacket_Test() {
	ServerPacket sp(SP_CHARACTER_LIST);

	sp.Encode1((BYTE)TA.GetCharacters().size()); // characters
	for (auto &chr : TA.GetCharacters()) {
		std::map<BYTE, Item> gequipped = chr.GetEquipped(0);
		std::map<BYTE, Item> equipped = chr.GetEquipped(1);

		sp.Encode4(chr.id); // ID
		sp.Encode1(chr.job_mask);
		sp.Encode1((BYTE)chr.level); // level
		sp.EncodeWStr1(chr.name); // name
		sp.EncodeWStr1(L"");
		sp.Encode2(chr.job);
		sp.Encode2(chr.skin);
		sp.Encode2(chr.hair);
		sp.Encode2(chr.face);
		sp.Encode2(chr.cloth);
		sp.Encode2(chr.gcolor);
		// guardian equip, max 15
		for (int i = 0; i < 15; i++) {
			sp.Encode2(gequipped.count(i) ? gequipped[i].itemID : 0);
		}
		// character equip, max 15
		for (int i = 0; i < 15; i++) {
			sp.Encode2(equipped.count(i) ? equipped[i].itemID : 0);
		}
		sp.Encode2(chr.map); // mapid
	}

	if (GetRegion() == TENVI_KR) {
		sp.Encode1(0);
	}

	sp.Encode1(TA.slot); // character slots
	SendPacket(sp);
}

// 0x07
void DeleteCharacter() {
	ServerPacket sp(SP_DELETE_CHARACTER_MSG);
	sp.Encode1(0); // 00491C61, error code 1,2,4
	SendPacket(sp);
}

// 0x08
void WorldSelectPacket() {
	ServerPacket sp(SP_WORLD_SELECT);
	SendPacket(sp);
}

// 0x09
int worlds = 1;
int channels = 5;
void WorldListPacket() {
	ServerPacket sp(SP_WORLD_LIST);
	sp.EncodeWStr2(L""); // 004938BC, Message

	if (GetRegion() == TENVI_JP || GetRegion() == TENVI_CN || GetRegion() == TENVI_KR) {
		sp.Encode1(0); // 004938C8, NetCafe
	}

	sp.Encode1(worlds); // 00493934, Number of Worlds
	for(int world = 0; world < worlds; world++) {
		sp.Encode1(2); // 00493979, World Mark
		sp.EncodeWStr1(L"Spica"); // 0049398A World Name
		sp.Encode8(0); // 004939AD

		sp.Encode1(1); // 004939BA
		{
			sp.EncodeWStr1(L"127.0.0.1"); // 004939D2, IP
			sp.Encode2(8787); // 004939F5, Port
		}

		sp.Encode1(channels); // 00493A0D, Number of channels
		for (int channel = 0; channel < channels; channel++) {
			sp.Encode1(channel + 1); // 00493A1E
			sp.Encode1(50); // 00493A28, Population (CH1 and CH2 are edited by client side lol)
		}
	}

	sp.Encode1(1); // 00493A6A
	sp.Encode1(0); // 00493A7B

	if (GetRegion() == TENVI_JP || GetRegion() == TENVI_CN) {
		sp.Encode1(0); // 00493A88
	}

	sp.Encode1(0); // 00493A92

	if (GetRegion() == TENVI_KR) {
		DelaySendPacket(sp);
		return;
	}
	SendPacket(sp);
}

// 0x0A not used in JP
void CharacterSelectInvitedPacket() {
	ServerPacket sp(SP_CS_INVITED);
	sp.Encode1(0); // 00491CEF, 0-2
	sp.Encode4(0); // 00491E20, unique code
	SendPacket(sp);
}

// 0x0B not used in JP
void CharacterSelectUnknownMsgPacket() {
	ServerPacket sp(SP_CS_KOREAN_MSG);
	sp.Encode1(1); // 00491AF9, show dialog
	sp.Encode1(0); // 00491B1E, not used
	SendPacket(sp);
}

// 0x0C
void GetGameServerPacket() {
	ServerPacket sp(SP_GET_GAME_SERVER);
	sp.Encode1(0); // error code
	sp.Encode4(0x0100007F); // IP
	sp.Encode2(8787); // port
	sp.Encode4(0);
	sp.Encode4(0);
	SendPacket(sp);
}

// 0x0D
void GetLoginServerPacket() {
	ServerPacket sp(SP_GET_LOGIN_SERVER);
	sp.Encode1(0); // error code
	sp.Encode4(0);
	sp.Encode4(0);
	sp.Encode4(0x0100007F); // IP
	sp.Encode2(8787); // port
	SendPacket(sp);
}

// 0x0E
void ConnectedPacket() {
	ServerPacket sp(SP_CONNECTED);
	sp.Encode1(0); // error code
	SendPacket(sp);
}

// 0x0F
void MapResetPacket() {
	ServerPacket sp(SP_MAP_RESET);
	sp.Encode1(0); // error code
	sp.Encode4(0);
	sp.Encode4(0);
	sp.Encode4(0);
	sp.Encode2(0);
	sp.Encode2(0);
	SendPacket(sp);
}

// 0x10
void ChangeMapPacket(WORD mapid, float x = 0, float y = 0) {
	ServerPacket sp(SP_MAP_CHANGE);
	sp.Encode1(0); // error code = 37
	sp.Encode2(mapid); // mapid
	sp.Encode1(0); // 1 = empty map?
	sp.Encode4(0);
	sp.EncodeFloat(x); // float value
	sp.EncodeFloat(y); // float value
	sp.Encode1(0);
	sp.Encode1(0);
	sp.Encode1(0); // disable item shop and park
	sp.Encode1(0);
	sp.Encode4(0);
	SendPacket(sp);
}

// 0x11
void CharacterSpawnPacket(TenviCharacter &chr, float x = 0, float y = 0) {
	ServerPacket sp(SP_CHARACTER_SPAWN);
	sp.Encode4(chr.id); // 0048DB9B id, where checks id?
	sp.EncodeFloat(x); // 0048DBA5, coordinate x
	sp.EncodeFloat(y); // 0048DBAF, corrdinate y
	sp.Encode1(chr.direction); // 0048DBB9, direction 0 = left, 1 = right
	sp.Encode1(chr.guardian_flag); // 0048DBC6, guardian, 0 = guardian off, 1 = guardian on
	sp.Encode1(1); // 0048DBD3, death, 0 = death, 1 = alive
	sp.Encode1(0); // 0048DBE0, battle, 0 = change channel OK, 1 = change channel NG
	sp.Encode4(0); // 0048DBFB, ???
	sp.Encode1(chr.job_mask); // 0048DC08
	sp.Encode1((BYTE)chr.level); // 0048DC2B

	if (GetRegion() == TENVI_HK || GetRegion() == TENVI_KRX) {
		sp.Encode1(1);
	}

	sp.EncodeWStr1(chr.name); // name
	sp.EncodeWStr1(L""); // guardian name
	sp.Encode1(chr.fly); // 0048DC8F fly, guardian move
	sp.Encode1(chr.aboard); // 0048DC9C, pilot move
	sp.Encode2(chr.job); // 0048DCA9
	sp.Encode2(chr.skin); // 0048DCB7
	sp.Encode2(chr.hair); // 0048DCC5
	sp.Encode2(chr.face); // 0048DCD3
	sp.Encode2(chr.cloth); // 0048DCE1
	sp.Encode2(chr.gcolor); // 0048DCEF
	sp.Encode1(chr.awakening); // 0048DCFD, awakening

	std::map<BYTE, Item> gequipped = chr.GetEquipped(0);
	std::map<BYTE, Item> equipped = chr.GetEquipped(1);

	// guardian equip
	for (int i = 0; i < 15; i++) {
		sp.Encode8(gequipped.count(i) ? gequipped[i].inventoryID : 0);
		sp.Encode2(gequipped.count(i) ? gequipped[i].itemID : 0);
	}

	// character equip
	for (int i = 0; i < 15; i++) {
		sp.Encode8(equipped.count(i) ? equipped[i].inventoryID : 0);
		sp.Encode2(equipped.count(i) ? equipped[i].itemID : 0);
	}

	sp.Encode2(0); // 0048DDC3
	sp.Encode2(0); // 0048DDD1
	sp.Encode2(0); // 0048DDE1
	sp.Encode2(0); // 0048DDF1
	sp.Encode2(0); // 0048DE01

	if (GetRegion() == TENVI_JP || GetRegion() == TENVI_CN) {
		sp.Encode1(0); // 0048DE11
		sp.Encode1(0); // 0048DE21
		sp.Encode1(0); // 0048DE35
	}

	sp.EncodeWStr1(L""); // guild
	sp.Encode1(0); // 0057B508
	sp.Encode1(0); // 0057B515
	sp.Encode1(0); // 0057B522
	sp.Encode1(0); // 0057B52F
	sp.Encode1(0); // 0057B53C
	sp.Encode1(0); // 0057B549
	sp.Encode1(0); // 0048DE7F
	sp.Encode1(0); // 0048DE8C
	sp.Encode1(0); // 0048DE9F
	sp.Encode1(0); // 0048DEAC
	sp.Encode1(0); // 0048DEB9
	sp.Encode1(0); // 0048E4F3
	sp.Encode4(0); // 0048E513
	sp.Encode1(0); // 0048E51D
	sp.Encode1(0); // 0048E5F9
	sp.Encode1(0); // 0048E60D
	sp.Encode1(0); // 0048E621
	sp.Encode4(0); // 0048E6A1
	sp.Encode2(0); // 0048E6AB
	sp.Encode4(0); // 0048E6C7
	sp.Encode4(0); // 0048E6D1
	sp.Encode4(0); // 0048E6DB
	sp.Encode4(0); // 0048E6E5
	sp.Encode4(0); // 0048E6EF
	sp.Encode1(0); // 0048E6F9
	sp.Encode1(chr.titleEquipped); // 0048E706
	sp.Encode4(0); // 0048E713
	sp.Encode4(0); // 0048E71D
	sp.Encode4(0); // 0048E727
	sp.Encode4(0); // 0048E731
	sp.EncodeWStr1(L""); // 0048E73F
	sp.Encode2(0); // 0048E74A
	sp.Encode2(0); // 0048E757
	sp.EncodeWStr1(L""); // 0048E768
	sp.Encode1(0); // 0048E773
	sp.Encode1(0); // 0048E780

	if (GetRegion() == TENVI_HK || GetRegion() == TENVI_KR || GetRegion() == TENVI_KRX) {
		sp.Encode1(0);
		sp.Encode1(0);
	}

	SendPacket(sp);
}

// 0x12
void RemoveObjectPacket(DWORD object_id) {
	ServerPacket sp(SP_REMOVE_OBJECT);
	sp.Encode4(object_id); // not only for character
	SendPacket(sp);
}

// 0x13
void DeathPacket(TenviCharacter& chr) {
	ServerPacket sp(SP_DEATH);
	sp.Encode4(chr.id);
	sp.Encode1(1); // 0 or 1, different ui
	SendPacket(sp);
}

// 0x14
void CreateObjectPacket(TenviRegen &regen) {
	ServerPacket sp(SP_CREATE_OBJECT);
	sp.Encode4(regen.id);
	sp.Encode2(regen.object.id); // npc, mob id
	sp.Encode1(0);

	if (GetRegion() == TENVI_KRX) {
		sp.Encode1(0);
	}

	sp.Encode4(0);
	sp.Encode1(regen.flip); // face right
	sp.Encode1(0); // fade in
	sp.Encode4(0);
	sp.Encode1(1); //show, if coordinate is far from your character, the object will be invisible
	sp.Encode2(0);
	sp.EncodeFloat(regen.area.left);
	sp.EncodeFloat(regen.area.bottom);
	sp.Encode2(0);
	sp.EncodeFloat(regen.area.left);
	sp.EncodeFloat(regen.area.top);
	sp.EncodeFloat(regen.area.right);
	sp.EncodeFloat(regen.area.bottom);
	sp.Encode1(0);
	sp.Encode1(0);

	if (GetRegion() != TENVI_JP) {
		sp.Encode1(0);
	}
	SendPacket(sp);
}

void WeatherPacket(BYTE type, float factor, BYTE red, BYTE green, BYTE blue) {
	ServerPacket sp(SP_WEATHER);
	sp.Encode1(type); // weather type
	sp.EncodeFloat(factor); // weather power
	sp.Encode1(red); // b
	sp.Encode1(green); // g
	sp.Encode1(blue); // r
	sp.Encode1(0xFF); // ??
	sp.Encode4(0); // brightness
	DelaySendPacket(sp);
}

// 0x1B
void ChatPacket(std::wstring msg) {
	TenviCharacter& chr = TA.GetOnline();
	ServerPacket sp(SP_PLAYER_CHAT);
	sp.Encode1(0); // 0048D7DD
	sp.EncodeWStr1(chr.name); // 0048D80D, character name
	sp.Encode1(1); // 0048D829
	sp.Encode1(0); // 0048D83E
	sp.EncodeWStr1(msg); // 0048D8FE, message
	SendPacket(sp);
}

// 0x20
void ActivateObjectPacket(TenviRegen &regen) {
	ServerPacket sp(SP_ACTIVATE_OBJECT);
	sp.Encode4(regen.id);
	sp.Encode1(3); // 1 = fade in, 2 = !, 3 = walk, 4 = dash
	SendPacket(sp);
}

// 0x21
void HitPacket(DWORD hit_from, DWORD hit_to, DWORD skill_id) {
	ServerPacket sp(SP_HIT);
	sp.Encode4(hit_from); // 004867C1
	sp.Encode4(hit_to); // 004867C8
	sp.Encode1(0); // 00470977, Knock back
	sp.Encode4(hit_from); // 00470984
	sp.Encode2(skill_id); // 0047098E
	sp.Encode1(1); // 0047099B, hit count
	sp.Encode4(1); // 004709AC, damage
	sp.Encode1(0); // 004709C1
	sp.Encode1(0); // 004709CE
	sp.Encode1(0); // 004709DB
	sp.Encode2(0); // 004709E8 skill id?
	sp.Encode1(0); // 004709F5
	SendPacket(sp);
}

// 0x23
void ShowObjectPacket(TenviRegen &regen) {
	ServerPacket sp(SP_SHOW_OBJECT);
	sp.Encode4(regen.id);
	sp.Encode1(1);
	sp.Encode1(1);
	sp.Encode2(0);
	sp.EncodeFloat(regen.area.left);
	sp.EncodeFloat(regen.area.bottom);
	SendPacket(sp);
}

// 0x25
void NPC_TalkPacket(DWORD npc_id, DWORD dialog) {
	ServerPacket sp(SP_NPC_TALK);
	sp.Encode4(npc_id);
	sp.Encode4(dialog);
	sp.Encode2(0);
	sp.EncodeWStr1(L"");
	DelaySendPacket(sp);
}

// 0x2F
void EquipItem(Item item) {
	TenviCharacter& chr = TA.GetOnline();
	ServerPacket sp(SP_EQUIP_ITEM);
	sp.Encode1(0);
	sp.Encode4(chr.id); // char id
	sp.Encode1(item.isCash);
	sp.Encode1(item.slot); // item slot
	sp.Encode8(item.inventoryID); // inventory id
	sp.Encode2(item.itemID);
	SendPacket(sp);
}

// 0x30
void UnequipItem(Item item) {
	TenviCharacter& chr = TA.GetOnline();
	ServerPacket sp(SP_UNEQUIP_ITEM);
	sp.Encode1(0);
	sp.Encode4(chr.id);
	sp.Encode1(item.isCash);
	sp.Encode1(item.slot);
	sp.Encode2(item.itemID);
	SendPacket(sp);
}

// 0x32
void EditInventory(Item item, BYTE isDelay=0) {
	ServerPacket sp(SP_EDIT_INVENTORY);
	sp.Encode1(item.type); // 장비(0), 기타(1), 퀘스트(2), 캐시(3), 카드(4)
	sp.Encode1(item.loc); // 몇 번째 슬롯
	sp.Encode1(1); // 004992BD 0이면 사라짐??
	sp.Encode8(item.inventoryID); // inventory ID
	sp.Encode2(item.itemID); // 아이템 id
	sp.Encode4(item.number); // 소지수
	sp.Encode1(0); // 업그레이드 수
	sp.Encode1(1); // should be 1
	sp.Encode1(0); // 교환가능횟수
	sp.Encode2(80); // 내구도
	sp.Encode1(0);
	sp.Encode1(0);
	sp.Encode4(0);
	sp.Encode4(0);
	sp.Encode4(0);
	sp.Encode1(0);
	sp.Encode1(0);
	sp.Encode1(0);
	if (isDelay) {
		DelaySendPacket(sp);
	}
	else {
		SendPacket(sp);
	}
}

// 0x3C
void InMapTeleportPacket(TenviCharacter &chr, float x=0, float y=0) {
	ServerPacket sp(SP_IN_MAP_TELEPORT);
	sp.Encode4(chr.id); // 00489222, character id
	sp.Encode1(1); // 0048923E, 0 = fall down, 1 = teleport to platform
	sp.EncodeFloat(x); // 0048924B, x
	sp.EncodeFloat(y); // 00489255, y
	sp.Encode1(0); // 0048925F, 0 = face left, 1 = face right
	sp.Encode1(0); // 00489269
	sp.Encode1(0); // 00489276
	sp.Encode1(1); // 00489283, 0 = no guardian, 1 = with guardian
	SendPacket(sp);
}

// 0x3D
void AccountDataPacket(TenviCharacter &chr) {
	ServerPacket sp(SP_ACCOUNT_DATA);
	sp.Encode4(0); // 00498E4F, ???
	sp.Encode4(chr.id); // 00498E5C, character id
	sp.EncodeWStr1(chr.name); // 00498E79, character name
	sp.EncodeWStr1(L""); // 00498EA5, ???
	sp.Encode1(chr.job_mask); // 00498ECD

	if (GetRegion() == TENVI_HK || GetRegion() == TENVI_KRX) {
		sp.Encode1(10); // unk
	}

	sp.Encode1((BYTE)chr.level); // 00498EF0
	sp.Encode8(1234); // 00498F0C, EXP
	sp.Encode8(chr.money); // 00498F28, Coin (Gold, Silver, Bronze)
	sp.Encode8(0); // 00498F44, ???
	sp.Encode1(0); // 00498F60
	sp.Encode1(0); // 00498F70

	if (GetRegion() == TENVI_JP || GetRegion() == TENVI_CN) {
		sp.Encode1(0); // 00498F80
	}

	{
		sp.EncodeWStr1(L"TENVI"); // 0057A877, Guild Name
		{
			sp.Encode1(0); // 0057B508
			sp.Encode1(0); // 0057B515
			sp.Encode1(0); // 0057B522
			sp.Encode1(0); // 0057B52F
			sp.Encode1(0); // 0057B53C
			sp.Encode1(0); // 0057B549
		}
	}
	sp.EncodeWStr1(L"chr unk1"); // 00498FA0

	if (GetRegion() == TENVI_JP) {
		sp.Encode1(0); // 00498FC8
		sp.Encode1(0); // 00498FD8
		sp.Encode1(0); // 00498FF0
	}

	// loop 4
	{
		sp.Encode1(5 * 8); // 00499024, Equip Slot
		sp.Encode1(5 * 8); // 00499024, Item Slot
		sp.Encode1(5 * 8); // 00499024, Cash Slot
		sp.Encode1(4 * 10); // 00499024, Card Slots

		if (GetRegion() == TENVI_HK || GetRegion() == TENVI_KR || GetRegion() == TENVI_KRX) {
			sp.Encode1(4); // unk
		}

	}
	sp.EncodeWStr1(chr.profile); // 00499044, Profile Message
	sp.Encode1(0); // 0049906C, ???
	sp.Encode1(0); // 004990B3
	// loop
	{
		/*
		sp.Encode1(0);
		sp.Encode2(0);
		sp.Encode1(0);
		*/
	}
	sp.Encode1(0); // 00499101, ???
	sp.Encode4(0); // 00499117
	sp.Encode1(0); // 00499124
	sp.Encode1(0); // 00499134, Married?
	sp.Encode4(0); // 00499144, Partner Character ID?
	sp.EncodeWStr1(L""); // 00499155, Partner Name?
	sp.Encode1(0); // 0049917D, Item Shop New Icon
	sp.Encode1(0); // 0049918D, Item Shop Box Icon
	sp.Encode1(0); // 0049919D, ???

	if (GetRegion() == TENVI_CN) {
		sp.Encode1(0);
	}

	SendPacket(sp);
}

// 0x3E
void MoneyPacket(TenviCharacter&chr) {
	ServerPacket sp(SP_UPDATE_MONEY);
	sp.Encode1(0);
	sp.Encode8(chr.money);
	SendPacket(sp);
}

// 0x41
void PlayerHitPacket(TenviCharacter& chr, DWORD hit_from, WORD damage) {
	ServerPacket sp(SP_PLAYER_HIT);
	sp.Encode4(1); // 0048693A
	sp.Encode4(chr.id); // 00486941
	sp.Encode4(hit_from); // 0045D825,
	sp.Encode2(0); // 0045D82F, skill id, 0: body attack
	sp.Encode1(1); // 0045D83C, hit count
	sp.Encode2(damage); // 0045D84D, damage
	sp.Encode2(64); // 0045D865 64: knockback, 32: no knockback, 30: miss 
	sp.Encode1(0); // 0045D872
	sp.Encode1(0); // 0045D87F
	sp.Encode2(0); // 0045D88C
	sp.Encode1(1); // 0045D899
	SendPacket(sp);
}

// 0x42
void PlayerLevelUpPacket(TenviCharacter &chr) {
	ServerPacket sp(SP_PLAYER_LEVEL_UP);
	sp.Encode4(chr.id); // 00488FD1, id
	sp.Encode1(chr.level + 1); // 00488FDB, 0 = rank up, others are level
	SendPacket(sp);
}

// 0x45
void PlayerSPPacket(TenviCharacter &chr) {
	ServerPacket sp(SP_PLAYER_STAT_SP);
	sp.Encode2(chr.sp);
	SendPacket(sp);
}
// 0x46
void PlayerAPPacket(TenviCharacter &chr) {
	ServerPacket sp(SP_PLAYER_STAT_AP);
	sp.Encode2(chr.ap);
	SendPacket(sp);
}

// 0x47
void PlayerStatPacket(TenviCharacter &chr) {
	ServerPacket sp(SP_PLAYER_STAT_ALL);
	sp.Encode2(chr.HP); // 004956F5, HP
	sp.Encode2(chr.maxHP); // 00495713, MAXHP
	sp.Encode2(chr.MP); // 0049572F, MP
	sp.Encode2(chr.maxMP); // 0049574B, MAXMP
	sp.Encode2(chr.stat_str); // 00495767, STR
	sp.Encode2(chr.stat_dex); // 00495783, DEX
	sp.Encode2(chr.stat_hp); // 0049579F, HP
	sp.Encode2(chr.stat_int); // 004957BB, INT
	sp.Encode2(chr.stat_mp); // 004957D7, MP
	sp.Encode2(988); // 004957F3, Physical damage Min
	sp.Encode2(1006); // 0049580F, Physical damage Max
	sp.Encode2(1000); // 0049582B, Physical attack
	sp.Encode2(2718); // 00495847, Magic attack
	sp.Encode2(1887); // 00495863, Defense
	sp.Encode2(9130); // 0049587F, Physical hit rate
	sp.Encode2(9763); // 004958A7, Magic hit rate
	sp.Encode2(129); // 004958CF, Avoid rate
	sp.Encode2(189); // 004958F7, Physical critical
	sp.Encode2(2279); // 0049591F, Magic critical

	if (GetRegion() == TENVI_KRX) {
		sp.Encode2(0);
	}

	sp.Encode2(131); // 00495947, Flight speed
	sp.Encode2(100); // 0049596F, Walk speed
	sp.Encode2(22); // 00495997, Fire resistance
	sp.Encode2(23); // 004959B3, Ice resistance

	if (GetRegion() != TENVI_KRX) {
		sp.Encode2(24); // 004959CF, Plant resistance
	}

	sp.Encode2(25); // 004959EB, Light resistance
	sp.Encode2(26); // 00495A07, Dark resistance

	if (GetRegion() != TENVI_KRX) {
		sp.Encode2(0); // 00495A23
		sp.Encode2(0); // 00495A42
		sp.Encode2(0); // 00495A61
		sp.Encode2(0); // 00495A80
		sp.Encode2(0); // 00495A9F
	}
	else {
		sp.EncodeFloat(0.0);
		sp.EncodeFloat(0.0);
		sp.EncodeFloat(0.0);
		sp.EncodeFloat(0.0);
		sp.EncodeFloat(0.0);
		sp.EncodeFloat(0.0);
		sp.EncodeFloat(0.0);
		sp.EncodeFloat(0.0);
		sp.EncodeFloat(0.0);
		sp.EncodeFloat(0.0);
		sp.EncodeFloat(0.0);
		sp.EncodeFloat(0.0);
		sp.EncodeFloat(0.0);
		sp.Encode4(0);
	}


	SendPacket(sp);
}

// 0x4A
void GuardianSummonPacket(TenviCharacter &chr, bool bSummon) {
	ServerPacket sp(SP_GUARDIAN_SUMMON);
	sp.Encode4(chr.id);
	sp.Encode1(bSummon ? 0x01 : 0x00);
	SendPacket(sp);
}

// 0x4B
void EmotionPacket(TenviCharacter &chr, BYTE emotion) {
	ServerPacket sp(SP_EMOTION);
	sp.Encode4(chr.id); // 0048608E, character id
	sp.Encode1(emotion); // 00486099, emotion
	SendPacket(sp);
}

// 0x4D
void EquipProfile(TenviCharacter& chr, DWORD itemID) {
	ServerPacket sp(SP_UPDATE_PROFILE);
	sp.Encode1(0);
	sp.Encode4(chr.id); // chr id
	sp.Encode1(1);
	sp.Encode2(itemID);
	SendPacket(sp);
}
void PlayerGenderPacket(TenviCharacter& chr, BYTE gender) {
	ServerPacket sp(SP_UPDATE_PROFILE);
	sp.Encode1(1);
	sp.Encode4(chr.id);
	sp.Encode2(gender);
	SendPacket(sp);
}
void UpdateProfile(std::wstring wText) {
	ServerPacket sp(SP_UPDATE_PROFILE);
	sp.Encode1(2);
	sp.EncodeWStr1(wText);
	SendPacket(sp);
}

// 0x54
void WorldMapUpdatePacket(BYTE area_code) {
	ServerPacket sp(SP_WORLD_MAP_UPDATE);
	sp.Encode1(area_code); // 00496E95
	sp.Encode1(0); // 00496EB7
	sp.Encode1(1); // 00496ED7
	sp.Encode1(area_code); // 00496EE7
	DelaySendPacket(sp);
}

void WorldMapUpdatePacketTest(BYTE area_code) {
	ServerPacket sp(SP_WORLD_MAP_UPDATE);
	sp.Encode1(area_code);

	for (int i = 1; i < 256; i++) {
		sp.Encode1((BYTE)i);
	}
	sp.Encode1(0);

	std::vector<BYTE> activated_area;

	activated_area.push_back(8); // Silva
	activated_area.push_back(2); // Libra
	activated_area.push_back(5); // TalliB1
	activated_area.push_back(6); // Minos

	if (GetRegion() != TENVI_HK) {
		activated_area.push_back(1); // BikiWinee
		activated_area.push_back(3); // Phantom
		activated_area.push_back(4); // Puchipochi
	}

	if (GetRegion() == TENVI_HK || GetRegion() == TENVI_KR || GetRegion() == TENVI_KRX) {
		activated_area.push_back(7); // Gaia Farm
	}

	sp.Encode1(activated_area.size()); // Number of Islands
	for (auto &v : activated_area) {
		sp.Encode1(v);
	}

	DelaySendPacket(sp);
}


// 0x5B
void PlayerRevivePacket(TenviCharacter &chr) {
	ServerPacket sp(SP_PLAYER_REVIVE);
	sp.Encode4(chr.id);
	SendPacket(sp);
}


// 0x5C
void EnterItemShopErrorPacket() {
	ServerPacket sp(SP_ITEM_SHOP_ERROR);
	sp.Encode1(1); // 004C93B9, error code 1-7
	SendPacket(sp);
}

// 0x63
void InitKeySet() {
	ServerPacket sp(SP_KEY_SET);
	sp.Encode1(0);
	sp.Encode1(0);
	SendPacket(sp);
}

// 0x66
void UpdateSkillPacket(TenviCharacter &chr, WORD skill_id) {
	ServerPacket sp(SP_UPDATE_SKILL);
	sp.Encode4(chr.id); // 00485E65, character id
	sp.Encode2(skill_id); // 00485E6F, skill id
	sp.Encode1(1); // 00485E7A, 0 = failed, 1 = success
	DelaySendPacket(sp);
}

// 0x6C
void InitSkillPacket(TenviCharacter &chr) {
	ServerPacket sp(SP_PLAYER_SKILL_ALL);
	sp.Encode1((BYTE)chr.skill.size()); // 0049977E, number of skills

	for (auto v : chr.skill) {
		sp.Encode1(1); // 00499792, idk
		sp.Encode2(v.id); // 0049979F, skill id
		sp.Encode1(v.level); // 004997AA, skill point
	}

	SendPacket(sp);
}

// 0xA6
void ShopPacket(DWORD npc_id, int currency, std::vector<ShopItem>& items) {
	ServerPacket sp(SP_SHOP);
	sp.Encode4(npc_id); // 00403097 npc id
	sp.Encode1(1); // 00403151 1: can repair
	sp.Encode1(1); // 00403165 sell item
	sp.Encode1(1); // 00403179
	sp.Encode2(currency); // 00403189 Currency (item id, -1 = coin)

	sp.Encode1(items.size()); // 004031A8 "count"
	for (auto& item : items) {
		sp.Encode2(item.itemID); // 004031BD "item id"
		sp.Encode2(item.count); // 004031C8 "item count"
		sp.Encode2(0); // 004031D3
		sp.Encode8(item.price); // 004031DE "money"
		sp.Encode4(0); // 004031EB title limit
		sp.Encode2(0); // 004031F5
		sp.Encode4(0); // 00403200
		sp.Encode2(0); // 0040320A
		sp.Encode2(0); // 00403217 level limit
		sp.EncodeFloat(0); // 0040322B quest limit
	}
	SendPacket(sp);
}

// 0xB0
void BankPacket(DWORD npc_id) {
	ServerPacket sp(SP_BANK);
	sp.Encode4(npc_id);
	SendPacket(sp);
}

// 0xB4
void MailPacket(DWORD npc_id) {
	ServerPacket sp(SP_MAIL);
	sp.Encode4(npc_id);
	SendPacket(sp);
}

// 0xBC
void AuctionPacket(DWORD npc_id) {
	ServerPacket sp(SP_AUCTION);
	sp.Encode4(npc_id);
	SendPacket(sp);
}

// 0xCC
void UseTelescope() {
	ServerPacket sp(SP_TELESCOPE);
	SendPacket(sp);
}

// 0xE0
DWORD boardID = 1;
enum BoardAction {
	Board_Spawn = 0,
	Board_Remove = 1,
	Board_AddInfo = 2,
};
void BoardPacket(BoardAction action, std::wstring owner = L"", std::wstring msg = L"", float x=0, float y=0) {
	ServerPacket sp(SP_BOARD);

	sp.Encode1(action); // 0048F653, 0 = spawn object, 1 = remove object, 2 = insert info

	switch (action) {
	case Board_Spawn: {
		sp.Encode4(boardID); // 0048AEF6 object id
		sp.Encode4(1337); // 0048AF00 ???
		sp.EncodeWStr1(owner); // 0048AF0E ???
		sp.EncodeWStr1(msg); // 0048AF1D message
		sp.EncodeFloat(x); // 0048AF28
		sp.EncodeFloat(y); // 0048AF32
		sp.Encode1(0); // 0048AF3C
		sp.Encode1(0); // 0048AF46 board type
		break;
	}
	case Board_Remove: {
		sp.Encode4(boardID); // object id
		break;
	}
	case Board_AddInfo: {
		sp.Encode4(boardID++); // object id
		sp.EncodeWStr1(msg); // 0048AF1D message
		break;
	}
	default: {
		// error
		return;
	}
	}
	SendPacket(sp);
}

// 0xE3
void HaveTitle(TenviCharacter& chr, std::vector<BYTE> titles) {
	ServerPacket sp(SP_TITLE);
	sp.Encode1(3); //
	sp.Encode4(chr.id); // player id
	sp.Encode1(titles.size()); // quantity
	for (auto& title : titles) {
		sp.Encode1(title);
		sp.Encode1(1);
	}
	sp.Encode1(1); // 선구자
	SendPacket(sp);
}
void EquipTitle(TenviCharacter& chr, BYTE code) {
	ServerPacket sp(SP_TITLE);
	sp.Encode1(0);
	sp.Encode4(chr.id); // player id
	sp.Encode1(code); // title code
	SendPacket(sp);
}

// 0xE4
void ShipPacket(DWORD process=-1) {
	ServerPacket sp(SP_SHIP);
	sp.Encode1(2);
	sp.Encode4(process);
	sp.Encode4(20000);
	SendPacket(sp);
}


// 0xE9
void EventAlarm(BYTE type) {
	ServerPacket sp(SP_EVENT);
	sp.Encode1(0); // 
	sp.Encode1(type); // type: 2, 3, 4, 5
	sp.Encode1(2); // 1 time 2 now 3 delete
	sp.Encode2(11); // Used in time
	SendPacket2(sp);
}
void EventCounter(DWORD time) {
	ServerPacket sp(SP_EVENT);
	sp.Encode1(4);
	sp.Encode1(0); // 1 start 2 finish
	sp.Encode1(0); // time 0 stop 1 start
	sp.Encode4(time * 1000); // time 1000 = 1sec
	SendPacket2(sp);
}

// ========== Functions ==================

void SpawnObjects(TenviCharacter &chr, WORD map_id) {
	for (auto &regen : tenvi_data.get_map(map_id)->GetRegen()) {
		CreateObjectPacket(regen);
		ShowObjectPacket(regen);
		ActivateObjectPacket(regen);
	}
}

void SetWeather(WORD map_id) {
	if (tenvi_data.data_weather.count(map_id)) {
		std::vector<Weather> weather = tenvi_data.data_weather[map_id];
		//Weather w = weather[rand() % weather.size()];
		Weather w = weather[0];
		for (auto& ww : weather) {
			if (w.type < ww.type) {
				// special weather first
				w = ww;
			}
		}
		WeatherPacket(w.type, w.factor, w.red, w.green, w.blue);
	}
}

void SetBoard(WORD map_id) {
	std::set<WORD> fields {1001, 1009, 1030, 1050, 1079, 2002, 2014, 2031, 2051, 3050, 4001,
		4002, 4003, 5003, 5028, 5042, 6001, 6009, 6031, 6085, 7010, 8003, 8004, 8028, 8048};
	if (fields.count(map_id) != 0) {
		std::vector<BoardInfo> infos = parse_board(map_id);
		for (auto& info : infos) {
			BoardPacket(Board_Spawn, L"Tenvi", StrToWstr(info.title), info.x, info.y);
			BoardPacket(Board_AddInfo, L"Tenvi", StrToWstr(info.message));
		}
	}
}

unsigned long int start_time = 0;
void SetTimer(DWORD map_id, DWORD time) {
	tenvi_data.get_map(map_id)->SetTimer(time);
	start_time = clock();
	EventCounter(time);
}

bool isFirst = true;
// go to map
void ChangeMap(TenviCharacter &chr, WORD map_id, float x, float y) {
	ChangeMapPacket(map_id, x, y);
	switch (map_id) {
	case MAPID_SHIP_PUCCHI:
	case MAPID_SHIP_MINOS:
	{
		if (isFirst) {
			isFirst = false;
			ShipPacket();
		}
		else{
			ShipPacket(36000);
		}
		chr.SetMapReturn(chr.map);
		chr.SetMap(map_id);
		break;
	}
	case MAPID_SHIP0:
	case MAPID_SHIP1:
	{
		ShipPacket();
		chr.SetMapReturn(chr.map);
		chr.map = map_id;
		SetTimer(map_id, 20);
		break;
	}
	case MAPID_ITEM_SHOP:
	case MAPID_PARK:
	{
		// do not change last map id
		chr.SetMapReturn(chr.map);
		break;
	}
	case MAPID_EVENT + 2:
	case MAPID_EVENT + 3:
	case MAPID_EVENT + 4:
	case MAPID_EVENT + 5:
	{
		// do not change last map id
		chr.SetMapReturn(chr.map);
		chr.guardian_flag = 0;
		break;

	}
	default:
	{
		chr.SetMapReturn(chr.map);
		chr.SetMap(map_id);
		break;
	}
	}
	SpawnObjects(chr, map_id);
	CharacterSpawnPacket(chr, x, y);
	HaveTitle(chr, chr.titles);
	SetWeather(map_id);
	SetBoard(map_id);
}

// enter map by login or something
void SetMap(TenviCharacter &chr, WORD map_id) {
	TenviMap* map = tenvi_data.get_map(map_id);
	TenviSpawnPoint spawn_point = map->FindSpawnPoint(0);
	ChangeMap(chr, map_id, spawn_point.x, spawn_point.y);
}

// enter map by portal
void UsePortal(TenviCharacter &chr, DWORD portal_id) {
	TenviPortal portal = tenvi_data.get_map(chr.map)->FindPortal(portal_id); // current map
	TenviPortal next_portal = tenvi_data.get_map(portal.next_mapid)->FindPortal(portal.next_id); // next map

	ChangeMap(chr, portal.next_mapid, next_portal.x, next_portal.y);
}

void GoTomb(TenviCharacter& chr) {
	DWORD return_id = tenvi_data.get_map(chr.map)->FindReturn();
	TenviPortal tomb_portal = tenvi_data.get_map(return_id)->FindTomb();
	ChangeMap(chr, return_id, tomb_portal.x, tomb_portal.y);
}

void ItemShop(TenviCharacter &chr, bool bEnter) {
	if (bEnter) {
		SetMap(chr, MAPID_ITEM_SHOP);
	}
	else {
		SetMap(chr, chr.map_return);
	}
}

void Park(TenviCharacter &chr, bool bEnter) {
	if (bEnter) {
		SetMap(chr, MAPID_PARK);
	}
	else {
		SetMap(chr, chr.map_return);
	}
}

void Event(TenviCharacter &chr, BYTE type) {
	if (type) {
		chr.aboard = 0;
		chr.fly = 0;
		chr.guardian_flag = 0;
		SetMap(chr, MAPID_EVENT + type);
	}
	else {
		SetMap(chr, chr.map_return);
	}
}

void RemoveFromInventory(BYTE loc, BYTE type) {
	ServerPacket sp(SP_EDIT_INVENTORY);
	sp.Encode1(type); // equip, other, cash, card
	sp.Encode1(loc);
	sp.Encode1(0);
	SendPacket(sp);
}

void InitEquip(TenviCharacter& chr) {
	std::map<BYTE, Item> gequipped = chr.GetEquipped(0);
	std::map<BYTE, Item> equipped = chr.GetEquipped(1);

	for (int i = 0; i < 15; i++) {
		if (gequipped.count(i)) {
			EditInventory(gequipped[i]);
			RemoveFromInventory(gequipped[i].loc, gequipped[i].type);
		}
	}
	for (int i = 0; i < 15; i++) {
		if (equipped.count(i)) {
			EditInventory(equipped[i]);
			RemoveFromInventory(equipped[i].loc, equipped[i].type);
		}
	}
	//RemoveFromInventory(0, 0);
	//RemoveFromInventory(3, 0);
	//RemoveFromInventory(4, 0);
}

void InitInventory(TenviCharacter& chr) {
	std::vector<Item> inventory = chr.GetInventory();
	for (Item& item : inventory) {
		EditInventory(item);
	}
}

std::pair<int, std::vector<ShopItem>> shop_items;
void EnterShop(WORD npc_id, WORD obj_id) {
	shop_items = parse_shop(obj_id);
	ShopPacket(npc_id, shop_items.first, shop_items.second);
}

void Die(TenviCharacter& chr) {
	chr.aboard = 0;
	chr.fly = 0;
	chr.guardian_flag = 0;
	DeathPacket(chr);
}

void CheckShip() {
	TenviCharacter& chr = TA.GetOnline();
	std::set<DWORD> ships{ MAPID_SHIP_PUCCHI, MAPID_SHIP_MINOS, MAPID_SHIP0, MAPID_SHIP1 };

	if (ships.find(chr.map) == ships.end()) {
		return;
	}
	if ((chr.map == MAPID_SHIP0 || chr.map == MAPID_SHIP1) && start_time != 0 && clock() - start_time > 1000) {
		start_time = clock();
		DWORD time = tenvi_data.get_map(chr.map)->Clock();
		EventCounter(time);
		if (time == 0) {
			start_time = 0;
			tenvi_data.get_map(chr.map)->time_now = -1;
			switch (chr.map) {
			case MAPID_SHIP0: {
				SetMap(chr, 6085);
				chr.map = 6085;
				ShipPacket(36000);
				InMapTeleportPacket(chr, 1060, 414);
				return;
			}
			case MAPID_SHIP1: {
				SetMap(chr, 4001);
				chr.map = 4001;
				ShipPacket(36000);
				InMapTeleportPacket(chr, 150, -76);
				return;
			}
			}
		}
	}

	if (chr.map == MAPID_SHIP_PUCCHI && chr.y < -1400 && chr.x < 400) {
		chr.map = MAPID_SHIP0;
		SetMap(chr, MAPID_SHIP0);
		InMapTeleportPacket(chr, -35, 224);
	}
	if (chr.map == MAPID_SHIP_MINOS && chr.y < -1400 && chr.x < 400) {
		chr.map = MAPID_SHIP1;
		SetMap(chr, MAPID_SHIP1);
		InMapTeleportPacket(chr, -35, 224);
	}

}

// ========== TENVI Server Main ==========
bool FakeServer(ClientPacket &cp) {
	CLIENT_PACKET header = cp.DecodeHeader();
	srand((unsigned int)time(NULL));
	CheckShip();

	switch (header) {
	// Select Character
	case CP_GAME_START: {
		DWORD character_id = cp.Decode4();
		BYTE channel = cp.Decode1();

		TA.Login(character_id);
		tenvi_data.parse_weather();

		for (auto &chr : TA.GetCharacters()) {
			if (chr.id == character_id) {
				chr.x = 0.0;
				chr.y = 0.0;
				GetGameServerPacket(); // notify game server ip
				ConnectedPacket(); // connected
				AccountDataPacket(chr);
				PlayerStatPacket(chr);
				PlayerSPPacket(chr);
				PlayerAPPacket(chr);
				InitSkillPacket(chr);
				InitEquip(chr);
				InitInventory(chr);
				InitKeySet();
				SetMap(chr, chr.map);
				//BoardPacket(Board_Spawn, L"Suhan", L"Picket");
				//BoardPacket(Board_AddInfo, L"Suhan", L"Non-commercial works");
				return true;
			}
		}
		return false;
	}
	// Create New Character
	case CP_CREATE_CHARACTER: {
		// TODO
		// 
		//std::wstring character_name = cp.DecodeWStr1();
		//BYTE job_mask = cp.Decode1(); // 0x11 to 0x24
		//WORD job_id = cp.Decode2(); // 4,5,6
		//WORD character_skin = cp.Decode2(); // 1,2,3
		//WORD character_hair = cp.Decode2();
		//WORD character_face = cp.Decode2();
		//WORD character_cloth = cp.Decode2();
		//WORD guardian_color = cp.Decode2();

		//WORD guardian_head = cp.Decode2();
		//WORD guardian_body = cp.Decode2();
		//WORD guardian_weapon = cp.Decode2();

		//std::map<BYTE, Item> guardian_equip, character_equip;
		//guardian_equip[_do] = TA.MakeItem(guardian_head);
		//guardian_equip[am] = TA.MakeItem(guardian_body);
		//guardian_equip[rh] = TA.MakeItem(guardian_weapon);

		//TA.AddCharacter(character_name, job_mask, job_id, character_skin, character_hair, character_face, character_cloth, guardian_color, 0, character_equip, guardian_equip);
		//CharacterListPacket_Test();
		return true;
	}
	// Delete Character
	case CP_DELETE_CHARACTER: {
		DWORD character_id = cp.Decode4();
		// not coded
		return true;
	}
	// Character Select to World Select
	case CP_BACK_TO_LOGIN_SERVER: {
		WorldSelectPacket(); // back to login server
		WorldListPacket(); // show world list
		return true;
	}
	// Game Server to Login Server
	case CP_LOGOUT: {
		GetLoginServerPacket();// notify login server ip
		ConnectedPacket(); // connected
		CharacterListPacket_Test();
		return true;
	}
	case CP_GUARDIAN_RIDE: {
		TenviCharacter& chr = TA.GetOnline();
		BYTE flag = cp.Decode1(); // on off
		chr.aboard = (flag ? 18 : 0);
		return true;
	}
	case CP_GUARDIAN_MOVEMENT: {
		TenviCharacter& chr = TA.GetOnline();
		float x = cp.DecodeFloat();
		chr.y = cp.DecodeFloat();
		chr.direction = (chr.x < x) ? 1 : 0;
		chr.x = x;
		cp.Decode1();
		chr.fly = (cp.Decode1() ? 10 : 0);
		return true;
	}
	case CP_USE_AP: {
		TenviCharacter &chr = TA.GetOnline();
		BYTE stat = cp.Decode1();
		BYTE amount = 1;
		if (GetRegion() == TENVI_KR || GetRegion() == TENVI_KRX) {
			amount = cp.Decode1();
		}
		chr.UseAP(stat, amount);
		PlayerAPPacket(chr);
		PlayerStatPacket(chr);
		return true;
	}
	case CP_EQUIP: {
		// 인벤토리에서 장비 장착
		TenviCharacter& chr = TA.GetOnline();
		BYTE type = cp.Decode1(); // 0: normal equip, 3: cash equip
		BYTE loc = cp.Decode1();
		BYTE slot = cp.Decode1();

		// type에 해당하는 inventory의 loc 자리에 있는 item을 slot에 넣는다.
		// slot이 0xFF이면 알아서 판단해야한다.
		Item item2equip = chr.GetItemByLoc(type, loc);
		writeDebugLog(std::to_string(item2equip.type));

		if (slot == 0xFF) {
			slot = item2equip.slot;
			if (slot == ri && chr.GetItemBySlot(ri, type).itemID && !(chr.GetItemBySlot(ri + 1, type).itemID)) {
				// 왼손 반지 착용 중이면 오른손 반지로 착용
				slot = ri + 1;
			}
		}
		if (slot == lh && FindIsTh(chr.GetItemBySlot(th, type).itemID)) {
			// 양손 무기 착용 중이면 왼손 무기 장착 불가
			EditInventory(item2equip);
			return true;
		}
		else if (FindIsTh(item2equip.itemID) && chr.GetItemBySlot(lh, type).itemID) {
			// 왼손 무기 착용 중이면 양손 무기 장착 불가
			EditInventory(item2equip);
			return true;
		}

		// item2Equip 을 장착한다
		// 기존에 착용 중인 장비가 있으면 인벤토리로 넣는다
		BYTE ring4 = slot == 4 ? 1 : 0;
		chr.EquipItem(item2equip, ring4);

		Item refreshSlot = chr.GetItemBySlot(slot, type);
		EquipItem(refreshSlot);

		Item refreshInventory = chr.GetItemByLoc(type, loc);
		if (refreshInventory.itemID) {
			EditInventory(refreshInventory, 1);
		}
		else {
			RemoveFromInventory(loc, type);
		}
		return true;

	}

	case CP_GUARDIAN_SUMMON: {
		TenviCharacter& chr = TA.GetOnline();
		chr.guardian_flag = cp.Decode1(); // on off
		GuardianSummonPacket(TA.GetOnline(), chr.guardian_flag ? true : false);
		return true;
	}
	case CP_UNEQUIP: {
		// 장비 해제
		TenviCharacter& chr = TA.GetOnline();
		BYTE type = cp.Decode1(); // 0: normal equip, 3: cash equip, 4: card
		BYTE slot = cp.Decode1();
		BYTE loc = cp.Decode1();

		Item item2unequip = chr.GetItemBySlot(slot, type);
		// 장비창에서 item2unequip을 삭제한다.
		UnequipItem(item2unequip);
		chr.UnequipItem(item2unequip, loc, slot == 4);

		// 인벤토리에 새로운 아이템을 추가한다.
		Item refreshInventory = chr.GetItemByLoc(type, loc);
		EditInventory(refreshInventory);

		return true;
	}
	case CP_SWITCH_RING: {
		TenviCharacter& chr = TA.GetOnline();
		BYTE r1 = cp.Decode1();
		BYTE r2 = cp.Decode1();
		BYTE isCash = cp.Decode1();
		BYTE type = isCash ? 3 : 0;
		if ((r1 == 3 && r2 == 4) || (r1 == 4 && r2 == 3)) {
			Item item1 = chr.GetItemBySlot(3, type);
			Item item2 = chr.GetItemBySlot(4, type);
			chr.SwitchRing(isCash);
			item1.slot = 4; item1.isCash = isCash;
			item2.slot = 3; item2.isCash = isCash;
			EquipItem(item1);
			EquipItem(item2);
		}
		return true;
	}
	case CP_EMOTION: {
		TenviCharacter &chr = TA.GetOnline();
		BYTE emotion = cp.Decode1();
		EmotionPacket(chr, emotion);
		return true;
	}
	case CP_MOVE_ITEM: {
		TenviCharacter& chr = TA.GetOnline();
		DWORD inventoryID = cp.Decode8();
		BYTE toLoc = cp.Decode1();

		Item item2move = chr.GetItemByInventoryID(inventoryID);
		Item existingItem = chr.GetItemByLoc(item2move.type, toLoc);
		
		BYTE fromLoc = item2move.loc;
		if ((item2move.type == 1 || item2move.type == 2) && item2move.itemID == existingItem.itemID) {
			// 기타 / 퀘스트 아이템은 합칠 수 있음
			chr.DeleteItem(inventoryID);
			existingItem.number += item2move.number;
			chr.ChangeItemNumber(existingItem.inventoryID, existingItem.number);
			RemoveFromInventory(fromLoc, item2move.type);
			EditInventory(existingItem, 1);
			return true;
		}
		chr.ChangeItemLoc(inventoryID, toLoc);
		RemoveFromInventory(fromLoc, item2move.type);
		if (existingItem.itemID) {
			chr.ChangeItemLoc(existingItem.inventoryID, fromLoc);
			existingItem.loc = fromLoc;
			EditInventory(existingItem, 1);

		}
		item2move.loc = toLoc;
		EditInventory(item2move, 1);

		return true;
	}
	case CP_UPDATE_PROFILE: {
		TenviCharacter& chr = TA.GetOnline();
		std::wstring wText = cp.DecodeWStr1();
		chr.profile = wText;
		UpdateProfile(wText);
		return true;
	}
	case CP_WORLD_MAP_OPEN:
	{
		BYTE area_code = cp.Decode1();
		//WorldMapUpdatePacket(area_code);
		WorldMapUpdatePacketTest(area_code);
		return true;
	}
	case CP_ITEM_SHOP: {
		BYTE flag = cp.Decode1();
		ItemShop(TA.GetOnline(), flag ? true : false);
		return true;
	}
	case CP_HIT: {
		TenviCharacter &chr = TA.GetOnline();

		DWORD hit_from = cp.Decode4();
		DWORD hit_to = cp.Decode4();
		WORD skill_id = cp.Decode2();

		if (hit_from > 0xFF && hit_to == chr.id) {
			return true;
		}

		if (chr.id != hit_to) {
			HitPacket(hit_from, hit_to, skill_id);
//			RemoveObjectPacket(hit_to);
			return true;
		}

		if (hit_to == chr.id) {
			WORD damage = 100;
			PlayerHitPacket(chr, hit_from, damage);
			chr.HP = (chr.HP <= damage) ? 0 : chr.HP - damage;
			chr.RefreshHPMP();
			PlayerStatPacket(chr);
			if (chr.HP == 0) {
				Die(chr);
			}

			return true;
		}

		return true;
	}
	case CP_USE_SP: {
		TenviCharacter &chr = TA.GetOnline();
		WORD skill_id = cp.Decode2();
		chr.UseSP(skill_id);
		UpdateSkillPacket(chr, skill_id);
		PlayerSPPacket(chr);
		return true;
	}
	case CP_USE_PORTAL: {
		TenviCharacter &chr = TA.GetOnline();
		DWORD portal_id = cp.Decode4();
		UsePortal(chr, portal_id);
		return true;
	}
	case CP_END_CAST: {
		cp.Decode4(); // chr.id
		DWORD type = cp.Decode2();
		switch (type) {
		case 0xF638: {
			// 천리경
			UseTelescope();
			return true;
		}
		case 0xEA5F: {
			// 마을 이동 주문서
			TenviCharacter& chr = TA.GetOnline();
			DWORD return_town = tenvi_data.get_map(chr.map)->FindReturnTown();
			// don't know why it makes error...
			// SetMap(chr, return_town);
			return true;
		}
		}
		return true;
	}
	case CP_TELLESCOPE_SELECT: {
		TenviCharacter& chr = TA.GetOnline();
		cp.Decode1();
		WORD itemID = cp.Decode2();

		ChatPacket(std::to_wstring(itemID));
		Item item = TA.MakeItem(chr, itemID, 1);
		if (item.itemID) {
			EditInventory(item);
		}
		chr.AddItem(item);

		return true;
	}
	case CP_PLAYER_REVIVE: {
		TenviCharacter& chr = TA.GetOnline();
		BYTE option = cp.Decode1(); // 1: tomb, 2: same pos, 3: revive potion
		PlayerRevivePacket(TA.GetOnline());
		chr.HP = chr.maxHP;
		chr.MP = chr.maxMP;
		chr.RefreshHPMP();
		PlayerStatPacket(chr);

		if (option == 1) {
			GoTomb(chr);
		}
		return true;
	}
	case CP_CHANGE_CHANNEL: {
		BYTE channel = cp.Decode1();
		return true;
	}
	case CP_NPC_TALK: {
		TenviCharacter& chr = TA.GetOnline();
		DWORD npc_id = cp.Decode4();
		DWORD unk2 = cp.Decode4();
		DWORD npc_type = cp.Decode4();
		DWORD unk3 = cp.Decode4();

		DWORD dialog = tenvi_data.get_map(chr.map)->FindNPCRegen(npc_id).dialog;
		WORD group = tenvi_data.get_map(chr.map)->FindNPCRegen(npc_id).group;
		WORD obj_id = tenvi_data.get_map(chr.map)->FindNPCRegen(npc_id).object.id;

		std::set<WORD> shop { 1005, 1006, 1041, 1042, 1108, 2009, 2010, 2011, 2012, 2031, 2033, 2088, 2090, 2095,
			2096, 2097, 2160, 2253, 2255, 2256, 3005, 3104, 3143, 3144, 3146, 3163, 4019, 4020, 4060, 4061, 4062,
			4065, 4066, 4068, 4070, 4071, 4072, 4116, 4119, 4134, 4192, 4220, 5010, 5011, 5012, 5013, 5014, 5015,
			5016, 5034, 5037, 5043, 5046, 5134, 5149, 5176, 5177, 5191, 5219, 5221, 5223, 5250, 6009, 6010, 6011,
			6012, 6013, 6017, 6018, 6019, 6023, 6024, 6025, 6026, 6027, 6028, 6037, 6041, 6044, 6045, 6046, 6047,
			6051, 6054, 6165, 6190, 6191, 6192, 7013, 8010, 8011, 8012, 8013, 8014, 8015, 8016, 8030, 8035, 8039,
			8043, 8214, 8217, 8218, 60033, 60039, 60070, 60071, 60083, 60084, 60094, 60110, 60125, 60126, 60127,
			60128, 60149, 60179 };

		if (shop.count(obj_id)) {
			EnterShop(npc_id, obj_id);
		}
		else if (dialog) {
			tenvi_data.get_map(chr.map)->pre_npc = npc_id;
			tenvi_data.get_map(chr.map)->pre_dialog = dialog;
			NPC_TalkPacket(npc_id, dialog);
		}
		switch (group) {
		case 20:
			MailPacket(npc_id);
			break;
		case 24:
			BankPacket(npc_id);
			break;
		case 26:
			AuctionPacket(npc_id);
			break;
		}		
//		ChatPacket(std::to_wstring(tenvi_data.get_map(chr.map)->FindNPCRegen(npc_id).group));
		return true;
	}
	case CP_NPC_ACTION: {
		TenviCharacter& chr = TA.GetOnline();
		DWORD action_id = cp.Decode4();
		DWORD pre_npc = tenvi_data.get_map(chr.map)->pre_npc, pre_dialog = tenvi_data.get_map(chr.map)->pre_dialog;
		DWORD dialog = parse_dialog(pre_dialog, action_id);
		if (dialog) {
			pre_dialog = dialog;
			NPC_TalkPacket(pre_npc, dialog);
		}
		return true;
	}
	case CP_DIVIDE_ITEM: {
		TenviCharacter& chr = TA.GetOnline();
		DWORD inventoryID = cp.Decode8();
		WORD number = cp.Decode2();

		Item item = chr.GetItemByInventoryID(inventoryID);
		item.number -= number;
		chr.ChangeItemNumber(inventoryID, item.number);
		Item new_item = TA.MakeItem(chr, item.itemID, number);
		chr.AddItem(new_item);
		EditInventory(item, 1);
		EditInventory(new_item, 1);
		return true;
	}
	case CP_BUY: {
		// TODO
		// 
		//TenviCharacter& chr = TA.GetOnline();
		//DWORD npc_id = cp.Decode4();
		//WORD itemID = cp.Decode2();
		//WORD num = cp.Decode2();
		//Item item = TA.MakeItem(itemID, num);
		//DWORD price = 0;

		//std::map<BYTE, Item>* inventory;
		//switch (FindType(itemID)) {
		//case 0: {
		//	inventory = &chr.inventory_equip;
		//	break;
		//}
		//case 1: {
		//	inventory = &chr.inventory_extra;
		//	break;
		//}
		//case 2: {
		//	inventory = &chr.inventory_quest;
		//	break;
		//}
		//case 3: {
		//	inventory = &chr.inventory_cash;
		//}
		//default: {
		//	inventory = &chr.inventory_card;
		//	break;
		//}
		//}
		//bool flag = true;
		//if (FindType(itemID) == 1 || FindType(itemID) == 2) {
		//	// 기타, 퀘스트 템인 경우 이미 가지고 있는지 확인해 개수만 늘리기
		//	for (int loc = 0; loc < 40; loc++) {
		//		if ((*inventory)[loc].itemID == itemID) {
		//			(*inventory)[loc].number += num;
		//			EditInventory(loc, (*inventory)[loc].inventoryID, (*inventory)[loc].itemID, (*inventory)[loc].type, (*inventory)[loc].number);
		//			flag = false;
		//			break;
		//		}
		//	}
		//}
		//if (flag) {
		//	// 빈 자리 있는지 확인
		//	for (int loc = 0; loc < 40; loc++) {
		//		if ((*inventory)[loc].itemID == 0) {
		//			(*inventory)[loc] = item;
		//			EditInventory(loc, item.inventoryID, item.itemID, item.type, num);
		//			flag = false;
		//			break;
		//		}
		//	}
		//}
		//if (flag) {
		//	// 빈 자리 없는 경우
		//	return true;
		//}

		//for (auto& item : shop_items.second) {
		//	// 가격 확인
		//	if (item.itemID == itemID) {
		//		price = item.price * num;
		//		break;
		//	}
		//}

		//if (shop_items.first == -1) {
		//	// 돈 차감
		//	chr.money -= price;
		//	MoneyPacket(chr);
		//}
		//else {
		//	// 재화 아이템 차감
		//	WORD currency = shop_items.first;
		//	for (auto& pair : chr.inventory_extra) {
		//		if (pair.second.itemID == currency) {
		//			chr.inventory_extra[pair.first].number -= price;
		//			writeDebugLog(std::to_string(price));
		//			if (chr.inventory_extra[pair.first].number == 0) {
		//				chr.inventory_extra[pair.first] = {};
		//				RemoveFromInventory(pair.first, 1);
		//			}
		//			else {
		//				EditInventory(pair.first, pair.second.inventoryID, pair.second.itemID, pair.second.type, pair.second.number);
		//			}
		//		}
		//	}
		//}
		return true;
	}
	case CP_SELL: {
		// TODO
		// 
		//TenviCharacter& chr = TA.GetOnline();
		//cp.Decode4();
		//DWORD inventoryID = cp.Decode8();
		//for (std::map<BYTE, Item>* inventory : { &chr.inventory_equip, &chr.inventory_cash, &chr.inventory_extra, &chr.inventory_quest, &chr.inventory_card }) {
		//	for (auto& pair : *inventory) {
		//		if (pair.second.inventoryID == inventoryID) {
		//			chr.money += pair.second.price;
		//			RemoveFromInventory(pair.first, pair.second.type);
		//			MoneyPacket(chr);
		//			(*inventory)[pair.first] = {};
		//			return true;
		//		}
		//	}
		//}
		return true;
	}
	case CP_PLAYER_CHAT: {
		TenviCharacter& chr = TA.GetOnline();
		BYTE type = cp.Decode1(); // 0 = map chat
		cp.Decode1(); // 1
		cp.Decode1(); // 0
		std::wstring message = cp.DecodeWStr1();

		ChatPacket(message);

		// command test
		if (message.length() && message.at(0) == L'@') {
			if (_wcsnicmp(message.c_str(), L"@map ", 5) == 0) {
				int map_id = _wtoi(&message.c_str()[5]);
				SetMap(TA.GetOnline(), map_id);
			}
			else if (_wcsnicmp(message.c_str(), L"@mob ", 5) == 0) {
				int npc_id = _wtoi(&message.c_str()[5]);
				TenviRegen regen = { 0, 0, 0, 0, 1, 0, 0, {chr.x, 0, 0, chr.y},  {npc_id} };
				CreateObjectPacket(regen);
				ShowObjectPacket(regen);
				ActivateObjectPacket(regen);
			}
			else if (_wcsicmp(message.c_str(), L"@event") == 0) {
				EventAlarm(2);
				EventAlarm(3);
//				EventAlarm(4);
				EventAlarm(5);
			}
			else if (_wcsicmp(message.c_str(), L"@death") == 0) {
				Die(chr);
			}
			else if (_wcsicmp(message.c_str(), L"@pos") == 0) {
				ChatPacket(L"x: " + std::to_wstring(chr.x) + L", y: " + std::to_wstring(chr.y));
			}

			return true;
		}

		return true;
	}
	case CP_CHANGE_TITLE: {
		TenviCharacter& chr = TA.GetOnline();
		BYTE code = cp.Decode1();
		EquipTitle(chr, code);
		chr.ChangeTitle(code);
		return true;
	}
	case CP_PARK: {
		BYTE flag = cp.Decode1();
		Park(TA.GetOnline(), flag ? true : false);
		return true;
	}
	case CP_PARK_BATTLE_FIELD: // ???
	case CP_EVENT: {
		BYTE type = cp.Decode1(); // 02: 낚시(62501), 03: 헬쉬(62502), 04: 아놀드(62503), 05: 나무(62504)
		Event(TA.GetOnline(), type);
		return true;
	}
	case CP_TIME_GET_TIME: {
		TenviCharacter& chr = TA.GetOnline();
		cp.Decode1(); // 0
		cp.Decode4(); // time
		return true;

	}
	default:
	{
		break;
	}
	}

	return false;
}