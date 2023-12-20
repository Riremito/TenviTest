#include"TenviData.h"
#include"rapidxml/rapidxml.hpp"
#include"rapidxml/rapidxml_utils.hpp"
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

bool TenviData::parse_weather() {
	std::string weather_xml = tenvi_data.get_xml_path() + +"\\" + tenvi_data.get_region_str() + "\\table\\weather.xml";
	writeDebugLog(weather_xml);
	OutputDebugStringA(("[Maple] xml = " + weather_xml).c_str());
	rapidxml::xml_document<> doc;

	try {
		rapidxml::file<> xmlFile(weather_xml.c_str());
		doc.parse<0>(xmlFile.data());
	}
	catch (...) {
		return false;
	}

	rapidxml::xml_node<>* root = doc.first_node();

	if (!root) {
		return false;
	}

	for (rapidxml::xml_node<>* group = root->first_node(); group; group = group->next_sibling()) {
		rapidxml::xml_node<>* weather, *map;
		std::vector<Weather> weatherVec;
		for (weather = group->first_node(); weather && strcmp("weather", weather->name()) == 0; weather = weather->next_sibling()) {
			Weather w = {};
			int red, green, blue;
			w.type = (BYTE)atoi(weather->first_attribute("type")->value());
			w.factor = (float)atof(weather->first_attribute("factor")->value());
			const char* color1 = weather->first_attribute("color1")->value();
			int res = sscanf_s(color1, "%d,%d,%d", &red, &green, &blue);
			float amp = 2.55;
			w.red = (BYTE)(red * amp);
			w.green = (BYTE)(green * amp);
			w.blue = (BYTE)(blue * amp);
			weatherVec.push_back(w);
		}

		for (map = weather; map && strcmp("map", map->name()) == 0; map = map->next_sibling()) {
			WORD mapID = (int)atoi(map->first_attribute("id")->value());
			writeDebugLog(std::to_string(mapID));
			data_weather[mapID] = weatherVec;
		}
	}
}

DWORD parse_dialog(DWORD dialog, DWORD action_id) {
	std::string dialog_str = (dialog < 10000) ? ("0" + std::to_string(dialog)) : std::to_string(dialog);
	std::string map_xml = tenvi_data.get_xml_path() + +"\\" + tenvi_data.get_region_str() + "\\dialog\\" + dialog_str + ".xml";
	rapidxml::xml_document<> doc;

	try {
		rapidxml::file<> xmlFile(map_xml.c_str());
		doc.parse<0>(xmlFile.data());
	}
	catch (...) {
		return 0;
	}

	rapidxml::xml_node<>* root = doc.first_node();

	if (!root) {
		return 0;
	}

	for (rapidxml::xml_node<>* child = root->first_node()->first_node(); child; child = child->next_sibling()) {
		if (strcmp("actions", child->name()) == 0) {
			for (rapidxml::xml_node<>* action = child->first_node(); action; action = action->next_sibling()) {
				int id = atoi(action->first_attribute("id")->value());
				if (id == action_id) {
					if (strcmp(action->first_attribute("type")->value(), "dialog") == 0) {
						return atoi(action->first_attribute("value")->value());
					}
					return 0;
				}
			}
		}
	}
	return 0;
}


std::string TenviData::get_xml_path() {
	return xml_path;
}

std::string TenviData::get_region_str() {
	return region_str;
}
