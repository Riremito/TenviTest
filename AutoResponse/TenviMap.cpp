#include"rapidxml/rapidxml.hpp"
#include"rapidxml/rapidxml_utils.hpp"
#include"TenviMap.h"
#include"TenviData.h"
#include"../EmuMainTenvi/ConfigTenvi.h"

TenviMap::TenviMap(DWORD mapid) {
	std::string region_str;
	switch (GetRegion()) {
	case TENVI_JP:
	case TENVI_CN:
	case TENVI_HK: {
		region_str = "KR";
		break;
	}
	default:
	{
		region_str = tenvi_data.get_region_str();
		break;
	}
	}
	id = mapid;
	LoadXML();
	LoadSubXML();
	LoadNPCDialog();
	Experimental();
}

rapidxml::xml_node<>* xml_find_dir(rapidxml::xml_node<>* parent, std::string name) {
	for (rapidxml::xml_node<>* child = parent->first_node(); child; child = child->next_sibling()) {
		if (name.compare(child->name()) == 0) {
			return child;
		}
	}
	return NULL;
}


bool TenviMap::LoadXML() {
	std::string mapid_str = (id < 10000) ? ("0" + std::to_string(id)) : std::to_string(id);
	std::string map_xml = tenvi_data.get_xml_path() + +"\\" + tenvi_data.get_region_str() + "\\map\\" + mapid_str + "_0.xml";
	rapidxml::xml_document<> doc;

	try {
		rapidxml::file<> xmlFile(map_xml.c_str());
		doc.parse<0>(xmlFile.data());
	}
	catch (...) {
		return false;
	}

	rapidxml::xml_node<>* root = doc.first_node();

	if (!root) {
		return false;
	}

	// spawn point
	rapidxml::xml_node<> *map_sp = xml_find_dir(root, "sp");
	if (map_sp) {
		for (rapidxml::xml_node<>* child = map_sp->first_node(); child; child = child->next_sibling()) {
			TenviSpawnPoint spawn_point = {};
			spawn_point.id = data_spawn_point.size(); // all map have only 1 spawn point?
			spawn_point.x = (float)atoi(child->first_attribute("x")->value());
			spawn_point.y = (float)atoi(child->first_attribute("y")->value());
			AddSpawnPoint(spawn_point);
		}
	}

	// portal
	rapidxml::xml_node<> *map_portal = xml_find_dir(root, "portal");
	if (map_portal) {
		for (rapidxml::xml_node<>* child = map_portal->first_node(); child; child = child->next_sibling()) {
			TenviPortal portal = {};
			portal.id = atoi(child->first_attribute("no")->value());
			portal.next_id = atoi(child->first_attribute("tno")->value());
			portal.next_mapid = atoi(child->first_attribute("tid")->value());
			portal.x = (float)atoi(child->first_attribute("x")->value());
			portal.y = (float)atoi(child->first_attribute("y")->value());
			AddPortal(portal);
		}
	}

	rapidxml::xml_node<>* map_attr = xml_find_dir(root, "attr");
	if (map_attr) {
		// tomb location id
		return_id = atoi(map_attr->first_attribute("return")->value());
		// return town id
		return_town_id = atoi(map_attr->first_attribute("returntown")->value());
	}

	return true;
}

bool TenviMap::LoadSubXML() {
	std::string mapid_str = (id < 10000) ? ("0" + std::to_string(id)) : std::to_string(id);
	std::string map_xml = tenvi_data.get_xml_path() + +"\\" + tenvi_data.get_region_str() + "\\npc\\regen\\" + mapid_str + "_0.xml";
	rapidxml::xml_document<> doc;

	try {
		rapidxml::file<> xmlFile(map_xml.c_str());
		doc.parse<0>(xmlFile.data());
	}
	catch (...) {
		return false;
	}

	rapidxml::xml_node<>* root = doc.first_node();

	if (!root) {
		return false;
	}

	// regen
	int regenCounter = 0;
	for (rapidxml::xml_node<>* node = root->first_node(); node; node = node->next_sibling()) {
		TenviRegen regen = {};
		regen.id = atoi(node->first_attribute("id")->value());
		regen.flip = atoi(node->first_attribute("flip")->value());
		regen.population = atoi(node->first_attribute("population")->value());

		for (rapidxml::xml_node<>* child = node->first_node(); child; child = child->next_sibling()) {
			if (strcmp("area", child->name()) == 0) {
				regen.area.left = atoi(child->first_attribute("left")->value());
				regen.area.top = atoi(child->first_attribute("top")->value());
				regen.area.right = atoi(child->first_attribute("right")->value());
				regen.area.bottom = atoi(child->first_attribute("bottom")->value());
				continue;
			}
			if (strcmp("id", child->name()) == 0) {
				regen.object.id = atoi(child->first_attribute("value")->value());
				//area.id = atoi(child->first_attribute("min")->value());
				//area.id = atoi(child->first_attribute("max")->value());
				continue;
			}
		}
		//for (int i = 0; i < regen.population; i++) {
		//	TenviRegen sub = regen;
		//	sub.id = regenCounter++;
		//	if (regen.population > 1) {
		//		sub.area.left = (regen.area.right - regen.area.left) * (i / (regen.population - 1)) + regen.area.left;
		//	}
		//	AddRegen(sub);
		//}
		AddRegen(regen);
	}
	return true;
}

bool TenviMap::LoadNPCDialog() {
	for (auto& regen : data_regen) {
		if (regen.object.id) {
			rapidxml::xml_document<> doc;
			std::string npc_id = (regen.object.id < 10000) ? ("0" + std::to_string(regen.object.id)) : std::to_string(regen.object.id);
			std::string npc_xml = tenvi_data.get_xml_path() + +"\\" + tenvi_data.get_region_str() + "\\npc\\" + npc_id + ".xml";
			regen.dialog, regen.group = 0, 0;
			try {
				rapidxml::file<> xmlFile(npc_xml.c_str());
				doc.parse<0>(xmlFile.data());
			}
			catch (...) {
				continue;
			}

			rapidxml::xml_node<>* root = doc.first_node();
			if (!root) {
				continue;
			}
			for (rapidxml::xml_node<>* child = root->first_node(); child; child = child->next_sibling()) {
				if (strcmp("npc", child->name()) == 0) {
					regen.group = atoi(child->first_attribute("group")->value());
					break;
				}
			}
			for (rapidxml::xml_node<>* child = root->first_node()->first_node(); child; child = child->next_sibling()) {
				if (strcmp("interactive", child->name()) == 0) {
					regen.dialog = atoi(child->first_attribute("dialog")->value());
					break;
				}
			}
			for (rapidxml::xml_node<>* child = root->first_node()->first_node()->first_node(); child; child = child->next_sibling()) {
				if (strcmp("friendship", child->name()) == 0) {
					regen.friendship = atoi(child->first_attribute("value")->value());
					break;
				}
			}
			continue;

		}
	}
	return true;
}

void TenviMap::Experimental() {
	std::vector<TenviRegen> new_regen;
	int regenCounter = 0;
	for (auto iter = data_regen.begin(); iter != data_regen.end(); iter++) {
		int cnt = (*iter).population;
		for (int i = 0; i < cnt; i++) {
			TenviRegen r = (*iter);
			r.id = regenCounter++;
			if (cnt > 1) {
				r.area.left = (r.area.right - r.area.left) * ((float)i / (cnt - 1)) + r.area.left;
			}
			new_regen.push_back(r);
		}
	}
	data_regen = new_regen;
}

DWORD TenviMap::GetID() {
	return id;
}

void TenviMap::AddSpawnPoint(TenviSpawnPoint &spawn_point) {
	data_spawn_point.push_back(spawn_point);
}

void TenviMap::AddPortal(TenviPortal &portal) {
	data_portal.push_back(portal);
}

void TenviMap::AddRegen(TenviRegen &regen) {
	data_regen.push_back(regen);
}

std::vector<TenviRegen>& TenviMap::GetRegen() {
	return data_regen;
}

TenviRegen& TenviMap::FindNPCRegen(DWORD npc_id) {
	for (auto& regen : data_regen) {
		if (regen.id == npc_id) {
			return regen;
		}
	}
	TenviRegen nullRegen = {};
	return nullRegen;
}

TenviSpawnPoint TenviMap::FindSpawnPoint(DWORD id) {
	for (auto &spawn_point : data_spawn_point) {
		if (spawn_point.id == id) {
			return spawn_point;
		}
	}

	TenviSpawnPoint fake_spawn_point = {};
	return fake_spawn_point;
}

TenviPortal TenviMap::FindPortal(DWORD id) {
	for (auto &portal : data_portal) {
		if (portal.id == id) {
			return portal;
		}
	}

	TenviPortal fake_portal = {};
	return fake_portal;
}

TenviPortal TenviMap::FindTomb() {
	for (auto& portal : data_portal) {
		if (portal.id == 254) {
			return portal;
		}
	}

	TenviPortal fake_portal = {};
	return fake_portal;
}

DWORD TenviMap::FindReturn() {
	if (return_id) {
		return return_id;
	}
	return id;
}

DWORD TenviMap::FindReturnTown() {
	if (return_town_id) {
		return return_town_id;
	}
	return id;
}

void TenviMap::SetTimer(DWORD t) {
	time_now = t;
}

DWORD TenviMap::Clock() {
	if (time_now > 0) {
		time_now--;
	}
	return time_now;
}

