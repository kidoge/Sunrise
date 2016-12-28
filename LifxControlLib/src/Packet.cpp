#include "Packet.h"
#include "HeaderContent.h"

Packet::Packet(std::shared_ptr<HeaderContent>header_ptr, 
			   std::shared_ptr<std::vector<uint8_t> > payload_ptr) : 
			   header(header_ptr), payload(payload_ptr) {
}

std::vector<uint8_t> Packet::getBytes() {
	std::vector<uint8_t> headerBytes = header->getBytes();
	std::vector<uint8_t> bytes;
	
	// Calculate packet size, including 2 bytes for packet size.
	uint16_t packet_size = headerBytes.size() + payload->size() + 2;
	bytes.reserve(packet_size);

	// Insert packet size t othe front of the header, in little endian.
	bytes.push_back(packet_size & 0x00FF); // LSB
	bytes.push_back((packet_size & 0xFF00) >> 8); // MSB

	// Insert header and payload data.
	bytes.insert(bytes.end(), headerBytes.begin(), headerBytes.end());
	bytes.insert(bytes.end(), payload->begin(), payload->end());
	
	return bytes;
}