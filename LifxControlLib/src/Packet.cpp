#include "Packet.h"

std::vector<unsigned char> Packet::getBytes() {
  std::vector<unsigned char> bytes;
  bytes.push_back(0);
  bytes.push_back(0);
  return bytes;
}