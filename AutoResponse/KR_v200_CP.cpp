#include"ClientPacket.h"

void SetClientPacketHeader_KR_v200() {
	BYTE *header = ClientPacket::GetOpcode();

	header[CP_VERSION] = 0x01;
	header[CP_CONNECT_RESPONSE] = 0x03;
	header[CP_GAME_START] = 0x04;
	header[CP_CREATE_CHARACTER] = 0x05;
	header[CP_DELETE_CHARACTER] = 0x06;
	header[CP_INVITED] = 0x07;
	header[CP_BACK_TO_LOGIN_SERVER] = 0x0A;
	header[CP_GAME_EXIT] = 0x0B;
	header[CP_EQUIP] = 0x0D;
	header[CP_UNEQUIP] = 0x0E;
	header[CP_SWITCH_RING] = 0x0F;
	header[CP_LOGOUT] = 0x09;
	header[CP_DROP_ITEM] = 0x10;
	header[CP_DROP_COIN] = 0x11;
	header[CP_MOVE_ITEM] = 0x13;
	header[CP_DIVIDE_ITEM] = 0x14;
	header[CP_GUARDIAN_RIDE] = 0x18;
	header[CP_GUARDIAN_MOVEMENT] = 0x19;
	header[CP_USE_AP] = 0x1A;
	header[CP_GUARDIAN_SUMMON] = 0x1B;
	header[CP_EMOTION] = 0x1C;
	header[CP_UPDATE_PROFILE] = 0x20;
	header[CP_WORLD_MAP_OPEN] = 0x23;
	header[CP_PICK_UP] = 0x37;
	header[CP_ITEM_SHOP] = 0x38;
	header[CP_HIT] = 0x44;
	header[CP_END_CAST] = 0x45;
	header[CP_USE_SP] = 0x46;
	header[CP_START_CAST] = 0x47;
	header[CP_USE_PORTAL] = 0x4F;
	header[CP_PLAYER_REVIVE] = 0x50;
	header[CP_CHANGE_CHANNEL] = 0x51;
	header[CP_NPC_TALK] = 0x54;
	header[CP_PLAYER_CHAT] = 0x56;
	header[CP_KEY_SET] = 0x5B;
	header[CP_NPC_ACTION] = 0x60;
	header[CP_BUY] = 0x61;
	header[CP_SELL] = 0x64;
	header[CP_TELLESCOPE_SELECT] = 0xAD;
	header[CP_PARK] = 0xC0;
	header[CP_PARK_BATTLE_FIELD] = 0xC1;
	header[CP_CHANGE_TITLE] = 0xC4;
	header[CP_EVENT] = 0xC6;
	header[CP_TIME_GET_TIME] = 0xC8;
	header[CP_WARP] = 0xCD;
	header[CP_CE] = 0xCE;
	header[CP_SORT_INVENTORY] = 0xD0;
}