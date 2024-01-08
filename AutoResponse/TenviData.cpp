#include"TenviData.h"
#include"rapidxml/rapidxml.hpp"
#include"rapidxml/rapidxml_utils.hpp"
#include"../EmuMainTenvi/ConfigTenvi.h"
#include<locale>
#include<codecvt>
#include<fstream>
#include<algorithm>
#include<queue>

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

std::string WstrToStr(const std::wstring& var) {
	static std::locale loc("");
	auto& facet = std::use_facet<std::codecvt<wchar_t, char, std::mbstate_t>>(loc);
	return std::wstring_convert<std::remove_reference<decltype(facet)>::type, wchar_t>(&facet).to_bytes(var);
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

std::pair<std::string, DWORD> parse_action(DWORD dialog, DWORD action_id) {
	std::string dialog_str = (dialog < 10000) ? ("0" + std::to_string(dialog)) : std::to_string(dialog);
	std::string map_xml = tenvi_data.get_xml_path() + +"\\" + tenvi_data.get_region_str() + "\\dialog\\" + dialog_str + ".xml";
	rapidxml::xml_document<> doc;

	try {
		rapidxml::file<> xmlFile(map_xml.c_str());
		doc.parse<0>(xmlFile.data());
	}
	catch (...) {
		return {};
	}

	rapidxml::xml_node<>* root = doc.first_node();

	if (!root) {
		return {};
	}

	for (rapidxml::xml_node<>* child = root->first_node()->first_node(); child; child = child->next_sibling()) {
		if (strcmp("actions", child->name()) == 0) {
			for (rapidxml::xml_node<>* action = child->first_node(); action; action = action->next_sibling()) {
				int id = atoi(action->first_attribute("id")->value());
				if (id == action_id) {
					if (strcmp(action->first_attribute("type")->value(), "dialog") == 0) {
						return { "dialog", atoi(action->first_attribute("value")->value()) };
					}
					if (strcmp(action->first_attribute("type")->value(), "function") == 0) {
						return { "function", atoi(action->first_attribute("value")->value()) };
					}
					return {};
				}
			}
		}
	}
	return {};
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

void TenviData::set_channel(BYTE nChannel) {
	channel = nChannel;
}
BYTE TenviData::get_channel() {
	return channel;
}

std::priority_queue<DWORD> TenviData::buff_no_queue;

void init_buff_queue() {
	TenviData::buff_no_queue = std::priority_queue<DWORD>();
	for (int i = 0; i < 3000; i++) {
		TenviData::buff_no_queue.push(i);
	}
}

void buff_no_free(DWORD buff_no) {
	TenviData::buff_no_queue.push(buff_no);
}

SkillInfo parse_skill_info(WORD skill_id, WORD level) {
	SkillInfo skill = {};
	skill.buff_no = TenviData::buff_no_queue.top();
	TenviData::buff_no_queue.pop();
	skill.skill_id = skill_id;
	skill.level = level;
	skill.rate = 200;
	skill.isBuff = 0;
	int temp_dot = 1;
	bool dotFlag = true;

	std::string str = std::to_string(skill_id);
	int precision = 5 - (5 < str.size() ? 5 : str.size());
	std::string skill_str = std::string(precision, '0').append(str);

	std::string filename = tenvi_data.get_xml_path() + +"\\" + tenvi_data.get_region_str() + "\\skill\\" + skill_str + ".xml";
	rapidxml::xml_document<> doc;
	try {
		rapidxml::file<> xmlFile(filename.c_str());
		doc.parse<0>(xmlFile.data());
	}
	catch (...) {
		return {};
	}

	rapidxml::xml_node<>* root = doc.first_node();
	if (!root) {
		return {};
	}
	for (rapidxml::xml_node<>* child = root->first_node(); child; child = child->next_sibling()) {
		if (strcmp(child->name(), "basic") == 0) {
			for (rapidxml::xml_node<>* sub = child->first_node(); sub; sub = sub->next_sibling()) {
				if (strcmp(sub->name(), "hostile") == 0) {
					skill.isHostile = atoi(sub->first_attribute("value")->value());
				}
				else if (strcmp(sub->name(), "normalbuff") == 0) {
					skill.isBuff = atoi(sub->first_attribute("value")->value());
				}
			}
		}
		else if (strcmp(child->name(), "levels") == 0) {
			for (rapidxml::xml_node<>* _level = child->first_node(); _level; _level = _level->next_sibling()) {
				if (atoi(_level->first_attribute("value")->value()) == level) {
					for (rapidxml::xml_node<>* sub = _level->first_node(); sub; sub = sub->next_sibling()) {
						if (strcmp(sub->name(), "mpcon") == 0) {
							skill.mpCon = atoi(sub->first_attribute("value")->value());
						}
						else if (strcmp(sub->name(), "mindamage") == 0) {
							skill.minDamage = atoi(sub->first_attribute("value")->value());
						}
						else if (strcmp(sub->name(), "maxdamage") == 0) {
							skill.maxDamage = atoi(sub->first_attribute("value")->value());
						}
						else if (strcmp(sub->name(), "RemainTick") == 0) {
							skill.isBuff = 1;
							skill.duration = atoi(sub->first_attribute("value")->value());
						}
						else if (strcmp(sub->name(), "buffinterval") == 0) {
							skill.isBuff = 1;
							dotFlag = false;
							temp_dot *= atoi(sub->first_attribute("value")->value());
						}
						else if (strcmp(sub->name(), "DotAttackCount") == 0) {
							skill.isBuff = 1;
							temp_dot *= atoi(sub->first_attribute("value")->value());
						}
						else if (strcmp(sub->name(), "MazeRate") == 0) {
							skill.isBuff = 1;
							skill.rate = atoi(sub->first_attribute("value")->value());
						}
						else if (strcmp(sub->name(), "buffsuccessrate") == 0) {
							skill.isBuff = 1;
							skill.rate = atoi(sub->first_attribute("value")->value());
						}
						else if (strcmp(sub->name(), "DotSuccessRate") == 0) {
							skill.isBuff = 1;
							skill.rate = atoi(sub->first_attribute("value")->value());
						}
					}
					break;
				}
			}
		}
	}
	if (temp_dot > 1) {
		if (dotFlag) {
			temp_dot *= 2000;
		}
		skill.duration = temp_dot;
	}
	if (skill.rate != 200) {
		srand((unsigned int)time(NULL));
		if ((rand() % 100) + 1 > skill.rate) {
			skill.isBuff = 0;
		}
	}
	return skill;
}