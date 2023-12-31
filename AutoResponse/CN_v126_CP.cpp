#include"ClientPacket.h"

void SetClientPacketHeader_CN_v126() {
	BYTE *header = ClientPacket::GetOpcode();

	header[CP_VERSION] = 0x01;
	header[CP_CONNECT_RESPONSE] = 0x03;
	header[CP_GAME_START] = 0x04;
	header[CP_CREATE_CHARACTER] = 0x05;
	header[CP_DELETE_CHARACTER] = 0x06;
	header[CP_INVITED] = 0x07;
	header[CP_BACK_TO_LOGIN_SERVER] = 0x0A;
	header[CP_LOGOUT] = 0x09;
	header[CP_USE_AP] = 0x1A;
	header[CP_EMOTION] = 0x1C;
	header[CP_WORLD_MAP_OPEN] = 0x23;
	header[CP_ITEM_SHOP] = 0x38;
	header[CP_USE_SP] = 0x45;
	header[CP_USE_PORTAL] = 0x4E;
	header[CP_CHANGE_CHANNEL] = 0x50;
	header[CP_PLAYER_CHAT] = 0x55;
	header[CP_PARK] = 0xBE;
	header[CP_PARK_BATTLE_FIELD] = 0xC1;
	header[CP_EVENT] = 0xC4;
	header[CP_TIME_GET_TIME] = 0xC6;
}