#include"TenviData.h"
#include"../EmuMainTenvi/ConfigTenvi.h"
#include<locale>
#include<codecvt>
#include<fstream>

TenviData tenvi_data; // global

void writeDebugLog(std::string str) {
	std::ofstream out;
	out.open("DebugLog.txt", std::ios_base::app);
	out << str << std::endl;
	out.close();
}


TenviMap* TenviData::get_map(DWORD id) {
	// map data is already loaded
	for (auto map : data_map) {
		if (map->GetID() == id) {
			return map;
		}
	}

	// load map data
	TenviMap *map = new TenviMap(id);
	data_map.push_back(map);
	return map;
}

void TenviData::set_xml_path(std::wstring path) {
	std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
	xml_path = converter.to_bytes(path);
	region_str = converter.to_bytes(GetRegionStr());
}

std::string TenviData::get_xml_path() {
	return xml_path;
}

std::string TenviData::get_region_str() {
	return region_str;
}
