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


BYTE FindType(DWORD itemID) {

	static std::map<std::string, BYTE> type_map = { {"cl", 0}, {"cp", 1}, {"nc", 2}, {"ri", 3},
		{"am", 6}, {"do", 7}, {"wp", 8}, {"lp", 9}, {"pp", 10}, 
		{"op", 11}, {"dc", 12}, {"rh", 13}, {"lh", 14}, {"rh,lh", 13} };

	std::string itemID_str = string_format("%05d", itemID);
	std::string item_xml = std::string("./") + "KR" + "/item/" + itemID_str + ".xml";

	rapidxml::xml_document<> doc;
	
	try {
		rapidxml::file<> xmlFile(item_xml.c_str());
		doc.parse<0>(xmlFile.data());

		rapidxml::xml_node<>* item;
		rapidxml::xml_node<>* basic = doc.first_node()->first_node();
		for (item = basic->first_node(); item; item = item->next_sibling()) {
			if (std::string(item->name()) == "slot") {
				return type_map[item->first_attribute("value")->value()];
			}
		}
		return false;
	}
	catch (...) {
		return false;
	}
	return false;
}

BYTE FindIsCash(DWORD itemID) {
	std::string itemID_str = string_format("%05d", itemID);
	std::string item_xml = std::string("./") + "KR" + "/item/" + itemID_str + ".xml";

	rapidxml::xml_document<> doc;

	try {
		rapidxml::file<> xmlFile(item_xml.c_str());
		doc.parse<0>(xmlFile.data());
		rapidxml::xml_node<>* item;
		rapidxml::xml_node<>* basic = doc.first_node()->first_node();
		for (item = basic->first_node(); item; item = item->next_sibling()) {
			if (std::string(item->name()) == "cash") {
				return std::stoi(item->first_attribute("value")->value());
			}
		}
		return false;
	}
	catch (...) {
		return false;
	}
	return false;

}
