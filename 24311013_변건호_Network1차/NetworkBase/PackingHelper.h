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
};
