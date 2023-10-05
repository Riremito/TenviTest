#ifndef __AUTORESPONSE_H__
#define __AUTORESPONSE_H__

#include"../Share/Simple/Simple.h"
#include"../Share/Hook/SimpleHook.h"
#include"TenviPacket.h"
#include"ClientPacket.h"
#include"ServerPacket.h"
#include"FakeServer.h"

bool AutoResponseHook();
void SendPacket(ServerPacket &sp);

#endif
