#ifndef __SERVERPACKET_H__
#define __SERVERPACKET_H__

// Server to Client (Recv, In)

#include<Windows.h>
#include<string>
#include<vector>

enum SERVER_PACKET {
	SP_BEGIN,
	SP_VERSION,
	SP_CRASH,
	SP_DEATH,
	SP_LOGIN_FAILED,
	SP_CHARACTER_SELECT,
	SP_CHARACTER_LIST,
	SP_DELETE_CHARACTER_MSG,
	SP_WORLD_SELECT,
	SP_WORLD_LIST,
	SP_WEATHER,
	SP_CS_INVITED,
	SP_CS_KOREAN_MSG,
	SP_GET_GAME_SERVER,
	SP_GET_LOGIN_SERVER,
	SP_CONNECTED,
	SP_MAP_RESET,
	SP_MAP_CHANGE,
	SP_CHARACTER_SPAWN,
	SP_REMOVE_OBJECT,
	SP_CREATE_OBJECT,
	SP_PLAYER_CHAT,
	SP_ACTIVATE_OBJECT,
	SP_HIT,
	SP_SHOW_OBJECT,
	SP_OBJECT_STAT,
	SP_IN_MAP_TELEPORT,
	SP_ACCOUNT_DATA,
	SP_PLAYER_HIT,
	SP_PLAYER_LEVEL_UP,
	SP_PLAYER_STAT_EXP,
	SP_PLAYER_STAT_SP,
	SP_PLAYER_STAT_AP,
	SP_PLAYER_STAT_ALL,
	SP_NPC_TALK,
	SP_UPDATE_MONEY,
	SP_EQUIP_ITEM,
	SP_UNEQUIP_ITEM,
	SP_DROP_ITEM,
	SP_PICK_UP,
	SP_PICK_UP_ERROR,
	SP_NEW_ITEM,
	SP_BUY_SELL_MSG,
	SP_REMOVE_DROP_ITEM,
	SP_EDIT_INVENTORY,
	SP_UPDATE_PROFILE,
	SP_GUARDIAN_SUMMON,
	SP_EMOTION,
	SP_WORLD_MAP_UPDATE,
	SP_PLAYER_REVIVE,
	SP_ITEM_SHOP_ERROR,
	SP_KEY_SET,
	SP_UPDATE_SKILL,
	SP_PLAYER_SKILL_ALL,
	SP_SKILL_COOLDOWN,
	SP_RECOVER_HP,
	SP_RECOVER_MP,
	SP_FRIEND_REQUEST,
	SP_GUILD_RANK_DOWN,
	SP_TELESCOPE,
	SP_INVENTORY_ERROR,
	SP_KICKOUT_MSG,
	SP_GM_MSG,
	SP_BOARD,
	SP_EVENT,
	SP_TITLE,
	SP_SHOP,
	SP_BANK,
	SP_MAIL,
	SP_AUCTION,
	SP_SHIP,
	SP_UNKNOWN,
	SP_END,
};

class ServerPacket {
private:
	static BYTE opcode[];

public:
	static BYTE* GetOpcode();

private:
	std::vector<BYTE> packet;

public:
	ServerPacket(SERVER_PACKET header);

	std::vector<BYTE>& get();
	void Encode1(BYTE val);
	void Encode2(WORD val);
	void Encode4(DWORD val);
	void EncodeWStr1(std::wstring val);
	void EncodeWStr2(std::wstring val);
	void Encode8(ULONGLONG val);
	void EncodeFloat(float val);
	std::vector<BYTE> EncodeAll(std::vector<BYTE> v);
};

void SetServerPacketHeader_JP_v127();
void SetServerPacketHeader_CN_v126();
void SetServerPacketHeader_HK_v102();
void SetServerPacketHeader_KRX_v107();
void SetServerPacketHeader_KR_v200();

#endif