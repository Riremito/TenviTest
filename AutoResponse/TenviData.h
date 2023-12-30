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

typedef struct {
	WORD itemID;
	int count;
	DWORD price;
} ShopItem;

typedef struct {
	WORD field;
	std::string title;
	std::string message;
	float x;
	float y;
} BoardInfo;

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
std::pair<int, std::vector<ShopItem>> parse_shop(WORD obj_id);
std::vector<BoardInfo> parse_board(WORD map_id);
void writeDebugLog(std::string str);
std::wstring StrToWstr(const std::string& var);
std::string WstrToStr(const std::wstring& wstr);
extern TenviData tenvi_data;

#endif