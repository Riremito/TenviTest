#include"Packet.h"

// header = byte
ServerPacket::ServerPacket(BYTE header) {
	Encode1(header);
}

ServerPacket::ServerPacket() {
	// for formatter
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
void ServerPacket::EncodeWStr(std::wstring val) {
	Encode1(val.length());
	for (size_t i = 0; i < val.length(); i++) {
		Encode2((WORD)val.at(i));
	}
}

void ServerPacket::Encode8(ULONGLONG val) {
	Encode4(val);
	Encode4(val >> 32);
}

void ServerPacket::EncodeFloat(float val) {
	Encode4((DWORD)val);
}