#include"rapidxml/rapidxml.hpp"
#include"rapidxml/rapidxml_utils.hpp"
#include"TenviItem.h"
#include"TenviData.h"
#include"../EmuMainTenvi/ConfigTenvi.h"
#include<map>

template<typename ... Args>
std::string string_format(const std::string& format, Args ... args) {
	size_t size = snprintf(nullptr, 0, format.c_str(), args ...) + 1; // Extra space for '\0' 
	if (size <= 0) {
		throw std::runtime_error("Error during formatting.");
	}
	std::unique_ptr<char[]> buf(new char[size]);
	snprintf(buf.get(), size, format.c_str(), args ...);
	return std::string(buf.get(), buf.get() + size - 1); // We don't want the '\0' inside }
}

rapidxml::xml_node<>* getNode(WORD itemID, std::string name) {
	std::string itemID_str = string_format("%05d", itemID);
	std::string item_xml = std::string("./") + tenvi_data.get_region_str() + "/item/" + itemID_str + ".xml";

	rapidxml::xml_document<> doc;
	try {
		rapidxml::file<> xmlFile(item_xml.c_str());
		doc.parse<0>(xmlFile.data());

		rapidxml::xml_node<>* item;
		rapidxml::xml_node<>* basic = doc.first_node()->first_node();
		for (item = basic->first_node(); item; item = item->next_sibling()) {
			if (std::string(item->name()) == name) {
				return item;
			}
		}
		return NULL;
	}
	catch (...) {
		return NULL;
	}

}

BYTE FindType(DWORD itemID) {
	static std::map<std::string, BYTE> type_map = { {"0", 0}, {"1", 0}, {"2", 1}, {"3", 2},
		{"4", 1}, {"5", 1}, {"6", 1}, {"11", 1}, {"12", 1}, {"13", 1}, {"14", 1}, {"16", 1}};

	if (FindIsCash(itemID)) {
		return 3;
	}

	rapidxml::xml_node<>* item = getNode(itemID, "type");
	if (item) {
		return type_map[item->first_attribute("value")->value()];
	}
	return NULL;
}


BYTE FindSlot(DWORD itemID) {
	static std::map<std::string, BYTE> slot_map = { {"cl", 0}, {"cp", 1}, {"nc", 2}, {"ri", 3}, {"fa", 5},
		{"am", 6}, {"do", 7}, {"wp", 8}, {"lp", 9}, {"pp", 10}, 
		{"op", 11}, {"dc", 12}, {"rh", 13}, {"lh", 14}, {"rh,lh", 13}};

	rapidxml::xml_node<>* item = getNode(itemID, "slot");
	if (item) {
		std::string slot_str = item->first_attribute("value")->value();
		if (slot_map.count(slot_str) != 0) {
			return slot_map[slot_str];
		}
	}
	return 15;
}

bool FindIsTh(DWORD itemID) {
	rapidxml::xml_node<>* item = getNode(itemID, "slot");
	if (item) {
		return std::string(item->first_attribute("value")->value()) == "rh,lh";
	}
	return false;
}

BYTE FindIsCash(DWORD itemID) {
	rapidxml::xml_node<>* item = getNode(itemID, "cash");
	if (item) {
		return std::stoi(item->first_attribute("value")->value());
	}
	return NULL;
}
