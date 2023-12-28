#ifndef __TEMVI_MAP_H__
#define __TEMVI_MAP_H__

#include<Windows.h>
#include<string>
#include<vector>

typedef struct {
	DWORD id;
	DWORD next_id;
	DWORD next_mapid;
	float x;
	float y;
} TenviPortal;

typedef struct {
	DWORD id; // no id?
	float x;
	float y;
} TenviSpawnPoint;

typedef struct {
	DWORD id;
	//DWORD object_id;
} TenviObject;

typedef struct {
	float left;
	float top;
	float right;
	float bottom;
} TenviArea;

typedef struct {
	DWORD id;
	DWORD delay;
	DWORD flip;
	DWORD once;
	DWORD population;
	DWORD dialog;
	DWORD group;
	TenviArea area;
	TenviObject object;
	BYTE friendship;
} TenviRegen;

class TenviMap {
private:
	DWORD id; // mapid
	std::vector<TenviSpawnPoint> data_spawn_point;
	std::vector<TenviPortal> data_portal;
	std::vector<TenviRegen> data_regen;
	DWORD return_id = 0;
	DWORD return_town_id = 0;

	bool LoadXML();
	bool LoadSubXML();
	bool LoadNPCDialog();

public:
	DWORD time_now = -1;
	DWORD pre_npc;
	DWORD pre_dialog;
	TenviMap(DWORD mapid);
	DWORD GetID();
	void AddSpawnPoint(TenviSpawnPoint &spawn_point);
	void AddPortal(TenviPortal &portal);
	void AddRegen(TenviRegen &regen);
	std::vector<TenviRegen>& GetRegen();
	TenviSpawnPoint FindSpawnPoint(DWORD id = 0);
	TenviPortal FindPortal(DWORD id);
	TenviPortal FindTomb();
	DWORD FindReturn();
	DWORD FindReturnTown();
	void SetTimer(DWORD t);
	DWORD Clock();
	TenviRegen& FindNPCRegen(DWORD npc_id);
	void ExperimentalNPC_MOB();
};

#endif