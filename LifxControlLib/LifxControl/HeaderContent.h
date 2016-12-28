#pragma once

#include <memory>
#include <vector>

struct HeaderContent {
public:
	std::vector<uint8_t> getBytes();

	/* frame */
	bool useTarget = false; // (tagged in docs) use frame address target field
	uint32_t source = 0; // ID for distinguishing clients

	/* frame address */
	uint64_t target = 0; // 6 byte MAC address, or 0 for all devices
	bool ack_required = false; // device returns acknowledgement packet
	bool res_required = false; // device returns response packet
	uint8_t sequence = 0; // ID for distinguishing responses for same client

	/* protocol header */
	uint16_t messageType = 0; // Type of message
};