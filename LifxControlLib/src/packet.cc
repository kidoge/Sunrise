#include "packet.h"
#include "header_content.h"

using lifx::Packet;
using lifx::HeaderContent;

Packet::Packet(const std::vector<uint8_t>& bytes) {
  std::vector<uint8_t>::const_iterator it = bytes.begin() + 2;
  header_ = std::shared_ptr<HeaderContent>(new HeaderContent(it));
  size_t header_size = header_->GetBytes().size();
  payload_ = std::make_shared<std::vector<uint8_t> >();
  payload_->resize(bytes.size() - header_size - 2);
  std::copy(it, bytes.end(), payload_->begin());
}
Packet::Packet(std::shared_ptr<HeaderContent>header,
               std::shared_ptr<std::vector<uint8_t> > payload) :
                 header_(header), payload_(payload) {
}

std::vector<uint8_t> Packet::getBytes() {
  std::vector<uint8_t> headerBytes = header_->GetBytes();
  std::vector<uint8_t> bytes;

  // Calculate packet size, including 2 bytes for packet size.
  size_t packet_size = headerBytes.size() + 2;
  if (payload_) {
    packet_size += payload_->size();
  }
  bytes.reserve(packet_size);

  // Insert packet size t othe front of the header, in little endian.
  bytes.push_back(packet_size & 0x00FF); // LSB
  bytes.push_back((packet_size & 0xFF00) >> 8); // MSB

  // Insert header and payload data.
  bytes.insert(bytes.end(), headerBytes.begin(), headerBytes.end());

  if (payload_) {
    bytes.insert(bytes.end(), payload_->begin(), payload_->end());
  }

  return bytes;
}

HeaderContent Packet::header() {
  return *header_;
}

std::vector<uint8_t> Packet::payload() {
  return *payload_;
}