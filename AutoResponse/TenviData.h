#ifndef __TENVI_DATA_H__
#define __TENVI_DATA_H__

#include"TenviMap.h"
#include<map>
#include<vector>

typedef struct {
	BYTE type;
	float factor;
	BYTE red;
	BYTE green;
	BYTE blue;
} Weather;

// loaded xml data list
class TenviData {
private:
	std::vector<TenviMap*> data_map;
	std::string xml_path;
	std::string region_str;

public:
	std::map<WORD, std::vector<Weather>> data_weather;
	TenviMap* get_map(DWORD id);
	void set_xml_path(std::wstring path);
	bool parse_weather();
	std::string get_xml_path();
	std::string get_region_str();
};

DWORD parse_dialog(DWORD dialog, DWORD action_id);
void writeDebugLog(std::string str);
extern TenviData tenvi_data;

#endif