#ifndef __FAKESERVER_H__
#define __FAKESERVER_H__

#include"ClientPacket.h"
#include"ServerPacket.h"
#include"TenviData.h"

#define MAPID_ITEM_SHOP 65535
#define MAPID_PARK 65534
#define MAPID_EVENT 62499
#define MAPID_SHIP_PUCCHI 4001
#define MAPID_SHIP0 4054

bool FakeServer(ClientPacket &cp);
// test
bool getIsFlag();
void LateInit_TA();
void WorldListPacket();
void CharacterSelectPacket();
void CharacterListPacket();
void CharacterListPacket_Test();

#endif