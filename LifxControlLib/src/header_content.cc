#include "header_content.h"

std::vector<uint8_t> HeaderContent::getBytes() {
  std::vector<uint8_t> bytes;

  // Last 8 bytes of protocol
  bytes.push_back(0);

  // Origin + Addressable + first 4 bytes of protocol
  uint8_t byte0 = 0b00010100;

  // Tagged
  if (useTarget) {
    byte0 |= 0b00100000;
  }
  bytes.push_back(byte0);

  // Source
  bytes.push_back(source & 0x000000FF);
  bytes.push_back((source & 0x0000FF00) >> 8);
  bytes.push_back((source & 0x00FF0000) >> 16);
  bytes.push_back((source & 0xFF000000) >> 24);

  // Target
  bytes.push_back(target & 0x00000000000000FF);
  bytes.push_back((target & 0x000000000000FF00) >> 8);
  bytes.push_back((target & 0x0000000000FF0000) >> 16);
  bytes.push_back((target & 0x00000000FF000000) >> 24);
  bytes.push_back((target & 0x000000FF00000000) >> 32);
  bytes.push_back((target & 0x0000FF0000000000) >> 40);
  bytes.push_back((target & 0x00FF000000000000) >> 48);
  bytes.push_back((target & 0xFF00000000000000) >> 56);

  // Reserved 6 bytes
  for (int idx = 0; idx < 6; ++idx) {
    bytes.push_back(0);
  }

  // Ack and Res
  bytes.push_back(static_cast<uint8_t>(ack_required << 1)
                  | static_cast<uint8_t>(res_required));

  // Sequence
  bytes.push_back(sequence);

  // Reserved 8 bytes
  for (int idx = 0; idx < 8; ++idx) {
    bytes.push_back(0);
  }

  bytes.push_back(messageType & 0x00FF);
  bytes.push_back((messageType & 0xFF00) >> 8);

  // Reserved 2 bytes
  for (int idx = 0; idx < 2; ++idx) {
    bytes.push_back(0);
  }

  return bytes;
}