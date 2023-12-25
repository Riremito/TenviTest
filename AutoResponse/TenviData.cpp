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

std::wstring StrToWstr(const std::string& var) {
	static std::locale loc("");
	auto& facet = std::use_facet<std::codecvt<wchar_t, char, std::mbstate_t>>(loc);
	return std::wstring_convert<std::remove_reference<decltype(facet)>::type, wchar_t>(&facet).from_bytes(var);
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

std::pair<int, std::vector<ShopItem>> parse_shop(WORD obj_id) {
	std::string shop_str = (obj_id < 10000) ? ("0" + std::to_string(obj_id)) : std::to_string(obj_id);
	std::string filename = tenvi_data.get_xml_path() + +"\\" + tenvi_data.get_region_str() + "\\shop\\" + shop_str + ".xml";
	rapidxml::xml_document<> doc;
	int currency = -1;
	std::vector<ShopItem> res = {};
	try {
		rapidxml::file<> xmlFile(filename.c_str());
		doc.parse<0>(xmlFile.data());
	}
	catch (...) {
		return {currency, res};
	}

	rapidxml::xml_node<>* root = doc.first_node();
	if (!root) {
		return {currency, res};
	}
	for (rapidxml::xml_node<>* child = root->first_node(); child; child = child->next_sibling()) {
		if (strcmp("sell", child->name()) == 0) {
			if (child->first_attribute("currency")) {
				currency = atoi(child->first_attribute("currency")->value());
			}
			for (rapidxml::xml_node<>* sell_item = child->first_node(); sell_item; sell_item = sell_item->next_sibling()) {
				WORD itemID = atoi(sell_item->first_attribute("item")->value());
				int count = atoi(sell_item->first_attribute("count")->value());
				DWORD price = atoi(sell_item->first_attribute("price")->value());
				res.push_back({ itemID, count, price });
			}
		}
	}
	return { currency, res };
}

std::vector<BoardInfo> parse_board(WORD map_id) {
	std::string filename = tenvi_data.get_xml_path() + +"\\" + tenvi_data.get_region_str() + "\\table\\" + "systemsign.xml";
	rapidxml::xml_document<> doc;
	std::vector<BoardInfo> boards;
	try {
		rapidxml::file<> xmlFile(filename.c_str());
		doc.parse<0>(xmlFile.data());
	}
	catch (...) {
		return boards;
	}

	rapidxml::xml_node<>* root = doc.first_node();
	if (!root) {
		return boards;
	}
	for (rapidxml::xml_node<>* sign = root->first_node(); sign; sign = sign->next_sibling()) {
		if (atoi(sign->first_node()->first_attribute("value")->value()) == map_id) {
			BoardInfo board;
			board.field = map_id;
			for (rapidxml::xml_node<>* sign_data = sign->first_node(); sign_data; sign_data = sign_data->next_sibling()) {
				const char* name = sign_data->name();
				if (strcmp(name, "title") == 0) {
					board.title = std::string(sign_data->first_attribute("value")->value());
				}
				else if (strcmp(name, "message") == 0) {
					board.message = std::string(sign_data->first_attribute("value")->value());
				}
				else if (strcmp(name, "centerx") == 0) {
					board.x = atof(sign_data->first_attribute("value")->value());

				}
				else if (strcmp(name, "centery") == 0) {
					board.y = atof(sign_data->first_attribute("value")->value());
				}
			}
			boards.push_back(board);
		}
	}
	return boards;
}

std::string TenviData::get_xml_path() {
	return xml_path;
}

std::string TenviData::get_region_str() {
	return region_str;
}
