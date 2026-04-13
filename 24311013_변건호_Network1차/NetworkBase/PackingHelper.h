#pragma once

#include "PacketHeader.h"
#include <string>

namespace PackingHelper
{
	//=== namespace ≥ª∫Œ «Ô∆€ ∏ﬁº≠µÂ ===//

	void Packing_Header(PacketHeader& header, const PacketType type, const uint16_t sizeofPacket);
	void Packing_CharArr(char* packetArr, const int arrSize, const std::string& packingTarget);


	//=== Packing «Ô∆€ ===//

	bool Packing_OnConnect(PacketOnConnect& packet, const std::string& name);
	void Packing_AssignID(PacketAssignID& packet, const int ID);
	bool Packeting_RequestChat(PacketRequestChat& packet, const std::string& chat);
	bool Packing_BroadcastChat(PacketBroadcastChat& packet, const std::string& name, const std::string& chat);


	//=== ≥ª∫Œ æ»¿¸ √ﬂ√‚ «Ô∆€ ===//

	std::string SafeGetString(const char* arr, const int maxSize);
	bool IsValidPacket(const PacketHeader& header, const PacketType type);


	//=== Unpack «Ô∆€ ===//

	bool Unpack_OnConnect(const void* buffer, std::string& out_name);
	bool Unpack_AssignID(const void* buffer, int& out_ID);
	bool Unpack_RequestChat(const void* buffer, std::string& out_chat);
	bool Unpack_BroadcastChat(const void* buffer, std::string& out_name, std::string& out_chat);

};
