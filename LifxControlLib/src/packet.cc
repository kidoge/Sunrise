#include "packet.h"
#include "header_content.h"

Packet::Packet(std::shared_ptr<HeaderContent>header,
               std::shared_ptr<std::vector<uint8_t> > payload) :
                 header_(header), payload_(payload) {
}

std::vector<uint8_t> Packet::getBytes() {
  std::vector<uint8_t> headerBytes = header_->GetBytes();
  std::vector<uint8_t> bytes;

  // Calculate packet size, including 2 bytes for packet size.
  uint16_t packet_size = headerBytes.size() + payload_->size() + 2;
  bytes.reserve(packet_size);

  // Insert packet size t othe front of the header, in little endian.
  bytes.push_back(packet_size & 0x00FF); // LSB
  bytes.push_back((packet_size & 0xFF00) >> 8); // MSB

  // Insert header and payload data.
  bytes.insert(bytes.end(), headerBytes.begin(), headerBytes.end());
  bytes.insert(bytes.end(), payload_->begin(), payload_->end());

  return bytes;
}