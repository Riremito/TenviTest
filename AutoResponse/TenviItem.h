#ifndef __TEMVI_ITEM_H__
#define __TEMVI_ITEM_H__

#include<Windows.h>
#include<string>
#include<vector>

BYTE FindType(DWORD itemID);
BYTE FindSlot(DWORD itemID);
BYTE FindGroup(DWORD itemID);
BYTE FindIsCash(DWORD itemID);
bool FindIsTh(DWORD itemID);
DWORD FindPrice(DWORD itemID);
BYTE FindRank(DWORD itemID);

#endif