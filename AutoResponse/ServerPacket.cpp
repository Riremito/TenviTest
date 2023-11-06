#include"ServerPacket.h"

// header = byte
ServerPacket::ServerPacket(BYTE header) {
	Encode1(header);
}

std::vector<BYTE>& ServerPacket::get() {
	return packet;
}

void ServerPacket::Encode1(BYTE val) {
	packet.push_back(val);
}
void ServerPacket::Encode2(WORD val) {
	packet.push_back(val & 0xFF);
	packet.push_back((val >> 8) & 0xFF);
}
void ServerPacket::Encode4(DWORD val) {
	packet.push_back(val & 0xFF);
	packet.push_back((val >> 8) & 0xFF);
	packet.push_back((val >> 16) & 0xFF);
	packet.push_back((val >> 24) & 0xFF);
}

// legnth = byte
void ServerPacket::EncodeWStr1(std::wstring val) {
	Encode1((BYTE)val.length());
	for (size_t i = 0; i < val.length(); i++) {
		Encode2((WORD)val.at(i));
	}
}

void ServerPacket::EncodeWStr2(std::wstring val) {
	Encode2((WORD)val.length());
	for (size_t i = 0; i < val.length(); i++) {
		Encode2((WORD)val.at(i));
	}
}

void ServerPacket::Encode8(ULONGLONG val) {
	Encode4((DWORD)val);
	Encode4((DWORD)(val >> 32));
}

void ServerPacket::EncodeFloat(float val) {
	unsigned int n;
	char strTmp[1024];
	memcpy(&n, &val, sizeof(float));
	sprintf_s(strTmp, "%08X", n);
	std::string temp = std::string(strTmp);
	std::string temp1 = temp.substr(0, 2) + temp.substr(2, 2);
	std::string temp2 = temp.substr(4, 2) + temp.substr(6, 2);
	Encode2((WORD)strtol(temp2.c_str(), NULL, 16));
	Encode2((WORD)strtol(temp1.c_str(), NULL, 16));
	// Encode4((DWORD)val);
}
