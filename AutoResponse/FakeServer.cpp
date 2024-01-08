#include"FakeServer.h"
#include"AutoResponse.h"
#include"Database.h"
#include"TenviItem.h"
#include"MobManager.h"
#include<map>
#include<set>
#include<ctime>
#include <clocale>
#include <locale>
#include<queue>

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
		sp.Encode2(0); // cloth?
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
		sp.Encode1(1);
		sp.Encode1(0);
		sp.Encode1(0);
		sp.Encode1(0);
		sp.EncodeWStr1(L"unknown");
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
	sp.Encode2(0); // 0048DCE1 cloth?
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

	sp.Encode1(0); // 0048DE7F guardian star?
	sp.Encode1(0); // 0048DE8C ?
	sp.Encode1(0); // 0048DE9F hp ap display?
	sp.Encode1(0); // 0048DEAC ?
	sp.Encode1(0); // 0048DEB9 buy sell on

	sp.Encode1(size(chr.buff)); // 0048E4F3 buff quantity
	for (auto& buff : chr.buff) {
		sp.Encode4(buff.first.buff_no); // buff_no
		sp.Encode2(buff.first.skill_id); // skill id
		sp.Encode1(buff.first.level); // level
		sp.Encode4(buff.first.duration - clock() + buff.second); // duration
		sp.Encode4(0); // ?
		sp.Encode1(1); // buff stack
	}

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
	DelaySendPacket(sp);
}

// 0x13
void DeathPacket(TenviCharacter& chr) {
	ServerPacket sp(SP_DEATH);
	sp.Encode4(chr.id);
	sp.Encode1(1); // 0 or 1, different ui
	SendPacket(sp);
}

// 0x14
void CreateObjectPacket(Mob mob) {
	ServerPacket sp(SP_CREATE_OBJECT);
	sp.Encode4(mob.regen.id);
	sp.Encode2(mob.regen.object.id); // npc, mob id
	sp.Encode1(0);

	if (GetRegion() == TENVI_KRX) {
		sp.Encode1(0);
	}

	sp.Encode4(0);
	sp.Encode1(mob.regen.flip); // face right
	sp.Encode1(0); // fade in
	sp.Encode4(0);
	sp.Encode1(1); //show, if coordinate is far from your character, the object will be invisible
	sp.Encode2(0);
	sp.EncodeFloat(mob.regen.area.left);
	sp.EncodeFloat(mob.regen.area.bottom);
	sp.Encode2(0);
	sp.EncodeFloat(mob.regen.area.left);
	sp.EncodeFloat(mob.regen.area.top);
	sp.EncodeFloat(mob.regen.area.right);
	sp.EncodeFloat(mob.regen.area.bottom);
	sp.Encode1(0);
	sp.Encode1(0);

	if (GetRegion() != TENVI_JP) {
		sp.Encode1(0);
	}
	SendPacket(sp);
}

// 0x18
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
void ActivateObjectPacket(DWORD mobID, BYTE state = 3) {
	ServerPacket sp(SP_ACTIVATE_OBJECT);
	sp.Encode4(mobID);
	sp.Encode1(state); // 1 = fade in, 2 = !, 3 = walk, 4 = dash, 5 = death?
	SendPacket(sp);
}

// 0x21
void HitPacket(DWORD hit_from, DWORD hit_to, DWORD skill_id, DWORD damage) {
	ServerPacket sp(SP_HIT);
	sp.Encode4(hit_from); // 004867C1
	sp.Encode4(hit_to); // 004867C8
	sp.Encode1(0); // 00470977, Knock back
	sp.Encode4(hit_from); // 00470984
	sp.Encode2(skill_id); // 0047098E skill id?
	sp.Encode1(1); // 0047099B, hit count
	sp.Encode4(damage); // 004709AC, damage
	sp.Encode1(0); // 004709C1
	sp.Encode1(0); // 004709CE
	sp.Encode1(0); // 004709DB
	sp.Encode2(skill_id); // 004709E8 skill id?
	sp.Encode1(0); // 004709F5
	SendPacket(sp);
}

// 0x23
void ShowObjectPacket(Mob mob, BYTE gravity=1) {
	ServerPacket sp(SP_SHOW_OBJECT);
	sp.Encode4(mob.regen.id);
	sp.Encode1(gravity);
	sp.Encode1(1);
	sp.Encode2(0);
	sp.EncodeFloat(mob.regen.area.left);
	sp.EncodeFloat(mob.regen.area.bottom);
	SendPacket(sp);
}

// 0x24
void ObjectStatPacket(Mob mob) {
	ServerPacket sp(SP_OBJECT_STAT);
	sp.Encode4(mob.regen.id);
	sp.Encode4(mob.HP); // hp
	sp.Encode2(3000); // mp
	sp.Encode4(mob.regen.vit); // max hp
	sp.Encode2(4000); // max mp
	sp.Encode2(100); // def
	sp.Encode2(100); // str
	sp.Encode2(100); // vit
	sp.Encode4(100); // agi
	sp.Encode2(100); // int
	sp.Encode2(100); // wis
	sp.Encode2(100); // dor
	sp.Encode2(100); // wsp speed
	sp.Encode2(100); // fsp
	SendPacket(sp);
}

// 0x25
void NPC_TalkPacket(DWORD npc_id, DWORD dialog, BYTE isDelay=1) {
	ServerPacket sp(SP_NPC_TALK);
	sp.Encode4(npc_id);
	sp.Encode4(dialog);
	sp.Encode2(0);
	sp.EncodeWStr1(L"");
	if (isDelay) {
		DelaySendPacket(sp);
	}
	else {
		SendPacket(sp);
	}
}

// 0x2C
void ApplyBuffPacket(DWORD id, SkillInfo skill) {
	ServerPacket sp(SP_APPLY_BUFF); // 0046FBEE
	sp.Encode4(id); // 00464F37
	sp.Encode4(skill.buff_no); // 0045D816
	sp.Encode2(skill.skill_id); // 0045D820
	sp.Encode1(skill.level); // 0045D82D
	sp.Encode4(skill.duration); // 0045D83A
	sp.Encode4(0); // 0045D844
	sp.Encode1(0); // 0045D84E
	SendPacket(sp);
}

// 0x2D
void CancelBuffPacket(DWORD id, DWORD buff_no) {
	ServerPacket sp(SP_CANCEL_BUFF);
	sp.Encode4(id);
	sp.Encode4(buff_no);
	SendPacket(sp);
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

// 0x36
void NewItemPacket(Item item) {
	ServerPacket sp(SP_NEW_ITEM);
	sp.Encode8(item.inventoryID);
	sp.Encode2(item.number);
	SendPacket(sp);
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
	sp.Encode1(tenvi_data.get_channel()); // 00499101, channel
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
void PlayerHitPacket(TenviCharacter& chr, DWORD hit_from, WORD damage, WORD skill_id) {
	ServerPacket sp(SP_PLAYER_HIT);
	sp.Encode4(1); // 0048693A
	sp.Encode4(chr.id); // 00486941
	sp.Encode4(hit_from); // 0045D825,
	sp.Encode2(skill_id); // 0045D82F, skill id, 0: body attack
	sp.Encode1(1); // 0045D83C, hit count
	sp.Encode2(damage); // 0045D84D, damage
	sp.Encode2(64); // 0045D865 64: knockback, 32: no knockback, 30: miss 
	sp.Encode1(0); // 0045D872
	sp.Encode1(0); // 0045D87F
	sp.Encode2(skill_id); // 0045D88C
	sp.Encode1(0); // 0045D899
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

// 0x48
void HP_RecoverPacket(TenviCharacter& chr, WORD amount) {
	ServerPacket sp(SP_RECOVER_HP); // 0046FBEE
	sp.Encode4(chr.id); // 004650A4
	sp.Encode2(amount); // 0043CC7D
	sp.Encode4(0); // 0043CC87
	sp.Encode2(0); // 0043CC91
	sp.Encode1(0); // 0043CC9E
	SendPacket(sp);
}

// 0x49
void MP_RecoverPacket(TenviCharacter& chr, WORD amount) {
	ServerPacket sp(SP_RECOVER_MP); // 0046FBEE
	sp.Encode4(chr.id); // 004650C7
	sp.Encode2(amount); // 0043CD88
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
void InitKeySet(std::vector<BYTE> all) {
	ServerPacket sp(SP_KEY_SET);
	for (BYTE& data : all) {
		sp.Encode1(data);
	}
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

// 0x67
void SkillCooldownPacket(WORD skill_id, DWORD time) {
	ServerPacket sp(SP_SKILL_COOLDOWN);
	sp.Encode1(1);
	sp.Encode2(skill_id);
	sp.Encode4(time);
	DelaySendPacket(sp);
}

// 0x6C
void InitSkillPacket(TenviCharacter &chr) {
	ServerPacket sp(SP_PLAYER_SKILL_ALL);
	sp.Encode1((BYTE)chr.GetSkill().size()); // 0049977E, number of skills

	for (auto v : chr.GetSkill()) {
		sp.Encode1(1); // 00499792, idk
		sp.Encode2(v.id); // 0049979F, skill id
		sp.Encode1(v.level); // 004997AA, skill point
	}

	SendPacket(sp);
}

// 0x6E
std::map<DWORD, Item> dropItems;
void DropItemPacket(TenviCharacter& chr, Item item, DWORD dropNo) {
	ServerPacket sp(SP_DROP_ITEM);
	sp.Encode4(dropNo); // 00488DC8 - drop item object id
	sp.Encode4(chr.id); // 00488DD7 - owner id
	sp.Encode2(item.itemID); // 00488DE1 - item code
	sp.Encode4(1); // 00488DEC - item amount
	sp.Encode4(0); // 00488DF6 - droper object id
	sp.Encode1(0); // 00488E00 - 1 = close picking
	sp.Encode1(0); // 00488E0D - 1 = translucent , need to close picking
	sp.Encode1(1); // 00488E1A - 0 = grow out | 1 = drop to right
	sp.EncodeFloat(0); // 00488E27  - drop vel x
	sp.EncodeFloat(-450); // 00488E31 - drop vel y
	sp.EncodeFloat(chr.x); // 00488E57 - drop to x
	sp.EncodeFloat(chr.y - 20); // 00488E61 - drop to y
	sp.Encode1(0); // 00488E6B - 1 = create and close effects
	sp.Encode4(chr.id); // 00488E7B - droper object id
	sp.Encode1(0); // 00488E85 - ?
	sp.Encode1(0); // 00488E92 - ?
	SendPacket(sp);
}
void DropCoinPacket(TenviCharacter& chr, DWORD dropNo, DWORD money) {
	ServerPacket sp(SP_DROP_ITEM);
	sp.Encode4(dropNo); // 00488DC8 - drop item object id
	sp.Encode4(chr.id); // 00488DD7 - owner id
	sp.Encode2(-1); // 00488DE1 - item code
	sp.Encode4(money); // 00488DEC - item amount
	sp.Encode4(0); // 00488DF6 - droper object id
	sp.Encode1(0); // 00488E00 - 1 = close picking
	sp.Encode1(0); // 00488E0D - 1 = translucent , need to close picking
	sp.Encode1(1); // 00488E1A - 0 = grow out | 1 = drop to right
	sp.EncodeFloat(0); // 00488E27  - drop vel x
	sp.EncodeFloat(-450); // 00488E31 - drop vel y
	sp.EncodeFloat(chr.x); // 00488E57 - drop to x
	sp.EncodeFloat(chr.y - 20); // 00488E61 - drop to y
	sp.Encode1(0); // 00488E6B - 1 = create and close effects
	sp.Encode4(chr.id); // 00488E7B - droper object id
	sp.Encode1(0); // 00488E85 - ?
	sp.Encode1(0); // 00488E92 - ?
	SendPacket(sp);
}

// 0x6F
void PickUpPacket(TenviCharacter& chr, DWORD drop_no) {
	ServerPacket sp(SP_PICK_UP);
	sp.Encode4(drop_no);
	sp.Encode4(chr.id);
	SendPacket(sp);
}

// 0x70
void PickUpErrorPacket(TenviCharacter& chr, DWORD drop_no) {
	ServerPacket sp(SP_PICK_UP_ERROR);
	sp.Encode4(drop_no);
	sp.Encode1(2); // 2: 인벤토리 꽉참
	sp.Encode1(dropItems[drop_no].type);
	SendPacket(sp);
}

// 0x7E
void HackedMessagePacket(BYTE msg_type, std::wstring msg) {
	// msg_type:
	// 12 = chat message
	// 1 = error message (middle)

	ServerPacket sp(SP_KICKOUT_MSG);
	sp.EncodeWStr1(L"@" + std::to_wstring(msg_type) + L" " + msg + L"\t");
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

// 0xC4
void ErrorInventoryFull() {
	ServerPacket sp(SP_INVENTORY_ERROR);
	sp.Encode2(0);
	sp.Encode1(1);
	SendPacket(sp);
}

// 0xCA
void BuyOrSellMessage(BYTE isBuy, Item item) {
	ServerPacket sp(SP_BUY_SELL_MSG); // 0046FBEE
	sp.Encode1(isBuy ? 5 : 6); // 005444F9 / 5 아이템 구매 / 6 판매
	sp.Encode1(0); // 00543B1E / 0=정상구매,판매 / 1~5 거래불가 문구 출력
	sp.Encode2(item.itemID); // 00543B88 / 아이템코드
	sp.Encode2(item.number); // 00543C68 / (판매시에만, 구매엔 필요없음) 판매 갯수
	SendPacket(sp);
}

// 0xCC
void UseTelescope() {
	ServerPacket sp(SP_TELESCOPE);
	SendPacket(sp);
}

// 0xE0
DWORD boardID;
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
		boardID = TA.GetObjectID();
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
MobManager mob_manager;
void SpawnObjects(TenviCharacter &chr, WORD map_id) {
	for (auto &regen : tenvi_data.get_map(map_id)->GetRegen()) {
		regen.area = { regen.area.left, 0, 0, regen.area.bottom };
		regen.id = TenviAccount::GetObjectID();
		Mob mob = mob_manager.AddMob(regen);
		CreateObjectPacket(mob);
		ShowObjectPacket(mob, 1);
		ActivateObjectPacket(mob.regen.id);
		ObjectStatPacket(mob);
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
		break;
	}
	case MAPID_SHIP0:
	case MAPID_SHIP1:
	{
		ShipPacket();
		chr.SetMapReturn(chr.map);
		SetTimer(map_id, 20);
		break;
	}
	case MAPID_EVENT + 2:
	case MAPID_EVENT + 3:
	case MAPID_EVENT + 4:
	case MAPID_EVENT + 5:
	{
		chr.guardian_flag = 0;
		break;
	}
	}
	chr.SetMap(map_id);
	mob_manager.ClearMobs();
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

void UseWarp(TenviCharacter& chr, WORD toMapID) {
	// toMapID 에서 tid = chr.map인 포털을 찾아야함
	WORD no = 0;
	switch (toMapID) {
	case 1001: // bikiwinee
		no = 3;
		break;
	case 2051: // libra
		no = 3;
		break;
	case 3001: // phantom
		no = 7;
		break;
	case 4001: // puchipochi
		break;
	case 5003: // talliB1
		no = 3;
		break;
	case 6001: // minos
		no = 1;
		break;
	case 7005: // gaia farm
		no = 3;
		break;
	case 8003: // silva
		no = 5;
		break;
	}
	TenviPortal next_portal = tenvi_data.get_map(toMapID)->FindPortal(no);
	ChangeMap(chr, toMapID, next_portal.x, next_portal.y);
}

void GoTomb(TenviCharacter& chr) {
	DWORD return_id = tenvi_data.get_map(chr.map)->FindReturn();
	TenviPortal tomb_portal = tenvi_data.get_map(return_id)->FindTomb();
	ChangeMap(chr, return_id, tomb_portal.x, tomb_portal.y);
}

void ItemShop(TenviCharacter &chr, bool bEnter) {
	if (bEnter) {
		chr.SetMapReturn(chr.map);
		SetMap(chr, MAPID_ITEM_SHOP);
	}
	else {
		SetMap(chr, chr.map_return);
		chr.SetMapReturn(0);
	}
}

void Park(TenviCharacter &chr, bool bEnter) {
	if (bEnter) {
		chr.SetMapReturn(chr.map);
		SetMap(chr, MAPID_PARK);
	}
	else {
		SetMap(chr, chr.map_return);
		chr.SetMapReturn(0);
	}
}

void Event(TenviCharacter &chr, BYTE type) {
	if (type) {
		chr.aboard = 0;
		chr.fly = 0;
		chr.guardian_flag = 0;
		chr.SetMapReturn(chr.map);
		SetMap(chr, MAPID_EVENT + type);
	}
	else {
		SetMap(chr, chr.map_return);
		chr.SetMapReturn(0);
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

void CheckMob() {
	Mob runner = mob_manager.Runner();
	if (runner.regen.id) {
		ActivateObjectPacket(runner.regen.id, 4);
	}

	Mob respawn = mob_manager.Respawn();
	if (respawn.regen.id) {
		CreateObjectPacket(respawn);
		ShowObjectPacket(respawn, 1);
		ActivateObjectPacket(respawn.regen.id, 1);
		ActivateObjectPacket(respawn.regen.id, 3);
		ObjectStatPacket(respawn);
	}
}

void CheckBuff() {
	TenviCharacter& chr = TA.GetOnline();
	for (auto it = chr.buff.begin(); it != chr.buff.end(); it++) {
		if (clock() - it->second > it->first.duration) {
			CancelBuffPacket(chr.id, it->first.buff_no);
			chr.buff.erase(it);
			break;
		}
	}
	for (auto& _mob : mob_manager.mobs) {
		Mob& mob = mob_manager.GetMob(_mob.first);
		for (auto it = mob.buff.begin(); it != mob.buff.end(); it++) {
			writeDebugLog(std::to_string(clock()) + ", " + std::to_string(it->second) + ", " + std::to_string(it->first.duration));
			if (clock() - it->second > it->first.duration) {
				CancelBuffPacket(_mob.first, it->first.buff_no);
				mob.buff.erase(it);
				break;
			}
		}
	}
}


// ========== TENVI Server Main ==========
bool FakeServer(ClientPacket &cp) {
	CLIENT_PACKET header = cp.DecodeHeader();
	//srand((unsigned int)time(NULL));
	//CheckShip();

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
				InitKeySet(chr.GetKeySet());
				SetMap(chr, chr.map);
				chr.isLoaded = true;
				//BoardPacket(Board_Spawn, L"Suhan", L"Picket");
				//BoardPacket(Board_AddInfo, L"Suhan", L"Non-commercial works");
				return true;
			}
		}
		return false;
	}
	// Create New Character
	case CP_CREATE_CHARACTER: {
		std::wstring character_name = cp.DecodeWStr1();
		BYTE job_mask = cp.Decode1(); // 0x11 to 0x24
		WORD job_id = cp.Decode2(); // 4,5,6
		WORD character_skin = cp.Decode2(); // 1,2,3
		WORD character_hair = cp.Decode2();
		WORD character_face = cp.Decode2();
		WORD character_cloth = cp.Decode2();
		WORD guardian_color = cp.Decode2();

		WORD guardian_head = cp.Decode2();
		WORD guardian_body = cp.Decode2();
		WORD guardian_weapon = cp.Decode2();

		TA.AddCharacter(character_name, job_mask, job_id, character_skin, character_hair, character_face, character_cloth,
			guardian_color, guardian_head, guardian_body, guardian_weapon);
		CharacterListPacket_Test();
		return true;
	}
	// Delete Character
	case CP_DELETE_CHARACTER: {
		DWORD character_id = cp.Decode4();
		TA.DeleteCharacter(character_id);
		CharacterListPacket_Test();
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
		TenviCharacter& chr = TA.GetOnline();
		chr.isLoaded = false;
		chr.SetMap(chr.map_return ? chr.map_return : chr.map);
		chr.SetMapReturn(0);

		GetLoginServerPacket();// notify login server ip
		ConnectedPacket(); // connected
		CharacterListPacket_Test();
		return true;
	}
	case CP_GAME_EXIT: {
		TenviCharacter& chr = TA.GetOnline();
		chr.SetMap(chr.map_return ? chr.map_return : chr.map);
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
		if (chr.x < x && chr.x != 0) {
			chr.direction = 1;
		}
		else if (chr.x > x && chr.x != 0) {
			chr.direction = 0;
		}
		chr.x = x;
		cp.Decode1();
		chr.fly = (cp.Decode1() ? 10 : 0);

		CheckMob();
		CheckBuff();
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
	case CP_DROP_ITEM: {
		TenviCharacter& chr = TA.GetOnline();
		DWORD inventoryID = cp.Decode8();
		BYTE unk = cp.Decode1();
		Item item = chr.GetItemByInventoryID(inventoryID);
		DWORD dropNo = TA.GetObjectID();
		DropItemPacket(chr, item, dropNo);
		dropItems[dropNo] = item;
		RemoveFromInventory(item.loc, item.type);
		chr.DeleteItem(inventoryID);
		return true;
	}
	case CP_DROP_COIN: {
		TenviCharacter& chr = TA.GetOnline();
		DWORD money = cp.Decode8();
		WORD dropNo = TA.GetObjectID();
		chr.ChangeMoney(chr.money - money);
		MoneyPacket(chr);
		DropCoinPacket(chr, dropNo, money);

		Item coin = {};
		coin.itemID = 0xFFFF;
		coin.number = money;
		dropItems[dropNo] = coin;
		return true;
	}
	case CP_PICK_UP: {
		TenviCharacter& chr = TA.GetOnline();
		DWORD dropNo = cp.Decode4();
		Item item2pick = dropItems[dropNo];
		Item existingItem = chr.GetItemByItemID(item2pick.itemID);
		BYTE num = item2pick.number;

		if (item2pick.itemID == 0xFFFF) {
			DWORD money = item2pick.number;
			chr.ChangeMoney(chr.money + money);
			MoneyPacket(chr);
			PickUpPacket(chr, dropNo);
			return true;
		}

		if ((item2pick.type == 1 || item2pick.type == 2) && existingItem.itemID) {
			// 기타, 퀘스트 템인 경우 이미 가지고 있으면 개수만 늘리기
			existingItem.number += num;
			chr.ChangeItemNumber(existingItem.inventoryID, existingItem.number);
			EditInventory(existingItem, 1);
		}
		else {
			// 인벤토리 공간이 부족한 경우
			int loc = chr.GetEmptyLoc(item2pick.type);
			if (loc == -1) {
				PickUpErrorPacket(chr, dropNo);
				return true;
			}
			item2pick.loc = loc;
			EditInventory(item2pick, 1);
			chr.AddItem(item2pick);
		}
		PickUpPacket(chr, dropNo);
		NewItemPacket(item2pick);
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
		chr.ChangeProfile(wText);
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
		WORD level = cp.Decode1();

		SkillInfo skill = parse_skill_info(skill_id, level);

		if (hit_from > 0xFF && hit_from != hit_to && hit_to == chr.id) {
			// 자동 회복??
			return true;
		}

		if (hit_to != chr.id) {
			// mob이 맞은 경우
			if (skill.isBuff) {
				skill.buff_no = TA.GetObjectID();
				std::vector<std::pair<SkillInfo, unsigned long int>>& buff = mob_manager.GetMob(hit_to).buff;
				for (auto it = buff.begin(); it != buff.end(); it++) {
					if (it->first.skill_id == skill.skill_id) {
						// 이미 가진 버프는 제거
						CancelBuffPacket(hit_to, it->first.buff_no);
						buff.erase(it);
						break;
					}
				}
				mob_manager.GetMob(hit_to).buff.push_back({ skill, clock() });
				ApplyBuffPacket(hit_to, skill);
			}
			WORD damage = (skill.isHostile ? skill.maxDamage : 0);
			HitPacket(hit_from, hit_to, skill_id, damage);
			bool isDead = mob_manager.Damage(hit_to, damage);
			ActivateObjectPacket(hit_to, 4);
			ObjectStatPacket(mob_manager.GetMob(hit_to));
			if (isDead) {
				ActivateObjectPacket(hit_to, 5);
				RemoveObjectPacket(hit_to);
			}
			return true;
		}

		if (hit_to == chr.id) {
			if (skill.isBuff) {
				skill.buff_no = TA.GetObjectID();
				for (auto it = chr.buff.begin(); it != chr.buff.end(); it++) {
					if (it->first.skill_id == skill.skill_id) {
						// 이미 가진 버프는 제거
						CancelBuffPacket(chr.id, it->first.buff_no);
						chr.buff.erase(it);
						break;
					}
				}
				chr.buff.push_back({ skill, clock() });
				ApplyBuffPacket(chr.id, skill);
			}
			WORD damage = (skill.isHostile ? skill.maxDamage : 0);
			PlayerHitPacket(chr, hit_from, damage, skill_id);
			chr.SetHP((chr.HP <= damage) ? 0 : chr.HP - damage);
			PlayerStatPacket(chr);
			if (chr.HP == 0) {
				Die(chr);
			}

			return true;
		}

		return true;
	}
	case CP_APPLY_DOT: { // not sure
		TenviCharacter& chr = TA.GetOnline();

		DWORD hit_from = cp.Decode4();
		cp.Decode4();
		WORD skill_id = cp.Decode2();
		BYTE level = cp.Decode1();
		cp.Decode8();
		DWORD hit_to = cp.Decode4();

		SkillInfo skill = parse_skill_info(skill_id, level);
		if (skill.duration < 2000) {
			skill.duration = 2000;
		}
		skill.buff_no = TA.GetObjectID();

		std::vector<std::pair<SkillInfo, unsigned long int>>& buff = ((hit_to == chr.id) ? chr.buff : mob_manager.GetMob(hit_to).buff);
		for (auto it = buff.begin(); it != buff.end(); it++) {
			if (it->first.skill_id == skill.skill_id) {
				// 이미 가진 버프는 제거
				CancelBuffPacket(hit_to, it->first.buff_no);
				buff.erase(it);
				break;
			}
		}
		buff.push_back({ skill, clock() });

		ApplyBuffPacket(hit_to, skill);
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
		TenviCharacter& chr = TA.GetOnline();
		cp.Decode4(); // chr.id
		DWORD type = cp.Decode2();
		BYTE level = cp.Decode1();

		switch (type) {
		case 0xF638: {
			// 천리경
			UseTelescope();
			return true;
		}
		case 0xEA5F: {
			// 마을 이동 주문서
			DWORD return_town = tenvi_data.get_map(chr.map)->FindReturnTown();
			// don't know why it makes error...
			// SetMap(chr, return_town);
			return true;
		}
		case 60000: {
			// HP 물약
			std::vector<WORD> healVal = { 0, 200, 380, 722, 1372, 2606, 0, 5, 25, 50, 25, 1509, 25, 2606 };
			SkillCooldownPacket(type, 5000);
			chr.HealHP(healVal[level]);
			HP_RecoverPacket(chr, healVal[level]);
			PlayerStatPacket(chr);
			return true;
		}
		case 60001: {
			// MP 물약
			std::vector<WORD> healVal = { 0, 250, 475, 903, 1715, 3528, 0, 0, 0, 0, 0, 1886, 3528 };
			SkillCooldownPacket(type, 5000);
			chr.HealMP(healVal[level]);
			MP_RecoverPacket(chr, healVal[level]);
			PlayerStatPacket(chr);
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
		chr.SetHP(chr.maxHP);
		chr.SetMP(chr.maxMP);
		PlayerStatPacket(chr);

		if (option == 1) {
			GoTomb(chr);
		}
		return true;
	}
	case CP_CHANGE_CHANNEL: {
		TenviCharacter& chr = TA.GetOnline();
		BYTE channel = cp.Decode1();
		tenvi_data.set_channel(channel);
		AccountDataPacket(chr);
		SetMap(chr, chr.map); // has no meaning
		return true;
	}
	case CP_MOB_DETECT: {
		DWORD mobID = cp.Decode4();
		mob_manager.Detect(mobID);
		ActivateObjectPacket(mobID, 2);
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
			if (obj_id == 60183) {
				NPC_TalkPacket(npc_id, dialog, 0);
			}
			else {
				NPC_TalkPacket(npc_id, dialog);
			}
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
		std::pair<std::string, DWORD> action = parse_action(pre_dialog, action_id);
		if (action.first == "dialog") {
			DWORD dialog = action.second;
			tenvi_data.get_map(chr.map)->pre_dialog = dialog;
			NPC_TalkPacket(pre_npc, dialog);
		}
		if (action.first == "function") {
			DWORD func = action.second;
			switch (func) {
			case 132: {
				Item cam = TA.MakeItem(chr, 60100, 1);
				if (cam.itemID) {
					chr.AddItem(cam);
					EditInventory(cam, 1);
				}
				else {
					HackedMessagePacket(1, L"인벤토리가 부족합니다.");
				}
				break;
			}
			case 258: {
				SetMap(chr, 4009);
				break;
			}
			case 259: {
				SetMap(chr, 4024);
				break;
			}
			case 260: {
				SetMap(chr, 4039);
				break;
			}
			}
		}
		return true;
	}
	case CP_KEY_SET: {
		// update key set
		TenviCharacter& chr = TA.GetOnline();
		std::string packetStr = cp.GetPacketStr();
		if (chr.isLoaded) {
			chr.KeySet(packetStr);
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
		TenviCharacter& chr = TA.GetOnline();
		DWORD npc_id = cp.Decode4();
		WORD itemID = cp.Decode2();
		WORD num = cp.Decode2();
		Item item2buy = TA.MakeItem(chr, itemID, num);
		DWORD price = 0;

		// 가격 확인
		for (auto& item : shop_items.second) {
			if (item.itemID == itemID) {
				price = item.price * num;
				break;
			}
		}
		Item existingItem = chr.GetItemByItemID(itemID);
		if ((FindType(itemID) == 1 || FindType(itemID) == 2) && existingItem.itemID) {
			// 기타, 퀘스트 템인 경우 이미 가지고 있으면 개수만 늘리기
			existingItem.number += num;
			chr.ChangeItemNumber(existingItem.inventoryID, existingItem.number);
			EditInventory(existingItem, 1);
		}
		else {
			// 인벤토리 공간이 부족한 경우
			if (!item2buy.itemID) {
				ErrorInventoryFull();
				return true;
			}
			EditInventory(item2buy, 1);
			chr.AddItem(item2buy);
		}

		if (shop_items.first == -1) {
			// 돈 차감
			chr.ChangeMoney(chr.money - price);
			MoneyPacket(chr);
		}
		else {
			// 재화 아이템 차감
			WORD currency = shop_items.first;
			while (price > 0) {
				Item currency_item = chr.GetItemByItemID(currency);
				if (currency_item.number <= price) {
					RemoveFromInventory(currency_item.loc, currency_item.type);
					chr.DeleteItem(currency_item.inventoryID);
					price -= currency_item.number;
				}
				else {
					currency_item.number -= price;
					EditInventory(currency_item, 1);
					chr.ChangeItemNumber(currency_item.inventoryID, currency_item.number);
					price = 0;
				}
			}
		}
		BuyOrSellMessage(1, item2buy);
		return true;
	}
	case CP_SELL: {
		TenviCharacter& chr = TA.GetOnline();
		cp.Decode4();
		DWORD inventoryID = cp.Decode8();
		
		Item item2sell = chr.GetItemByInventoryID(inventoryID);
		chr.ChangeMoney(chr.money + item2sell.price * item2sell.number);
		MoneyPacket(chr);
		chr.DeleteItem(inventoryID);
		RemoveFromInventory(item2sell.loc, item2sell.type);
		BuyOrSellMessage(0, item2sell);
		return true;
	}
	case CP_PLAYER_CHAT: {
		TenviCharacter& chr = TA.GetOnline();
		BYTE type = cp.Decode1(); // 0 = map chat
		cp.Decode1(); // 1
		cp.Decode1(); // 0
		std::wstring message = cp.DecodeWStr1();

		ChatPacket(message);
		HackedMessagePacket(12, L"[" + chr.name + L"] : " + message);

		// command test
		if (message.length() && message.at(0) == L'@') {
			if (_wcsnicmp(message.c_str(), L"@map ", 5) == 0) {
				int map_id = _wtoi(&message.c_str()[5]);
				SetMap(TA.GetOnline(), map_id);
			}
			else if (_wcsnicmp(message.c_str(), L"@mob ", 5) == 0) {
				//int npc_id = _wtoi(&message.c_str()[5]);
				//TenviRegen regen = {};
				//regen.area.left = chr.x;
				//regen.area.bottom = chr.y;
				//regen.id = 0xFFF0;
				//regen.object.id = npc_id;
				//CreateObjectPacket(regen);
				//ShowObjectPacket(regen);
				//ActivateObjectPacket(regen.id);
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
	case CP_SORT_INVENTORY: {
		TenviCharacter& chr = TA.GetOnline();
		BYTE method = cp.Decode1();
		BYTE type = cp.Decode1();
		chr.SortInventory(method, type);
		std::vector<Item> inventory = chr.GetInventory();
		for (int i = 0; i < 40; i++) {
			RemoveFromInventory(i, type);
		}
		for (Item& item : inventory) {
			if (item.type == type) {
				EditInventory(item);
			}
		}
		return true;
	}
	case CP_WARP: {
		TenviCharacter& chr = TA.GetOnline();
		WORD mapID = cp.Decode2();
		UseWarp(chr, mapID);
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