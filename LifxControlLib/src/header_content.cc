#include <exception>
#include "header_content.h"
#include "little_endian_helper.h"

using lifx::MessageTypes;
using lifx::HeaderContent;

void CheckReserved(std::vector<uint8_t>::const_iterator &it, size_t size) {
  for (int idx = 0; idx < size; ++idx) {
    if (*it != 0) {
      throw std::invalid_argument("bytes has invalid format.");
    }
    it++;
  }
}

HeaderContent::HeaderContent(std::vector<uint8_t> bytes) {
  auto it = bytes.begin();
  Init(it);
}

HeaderContent::HeaderContent(std::vector<uint8_t>::const_iterator &it) {
  Init(it);
}

std::vector<uint8_t> HeaderContent::GetBytes() const {
  std::vector<uint8_t> bytes;

  // Last 8 bytes of protocol
  bytes.push_back(0);

  // Origin + Addressable + first 4 bytes of protocol
  uint8_t byte0 = 0b00010100;

  // Tagged
  if (use_target_) {
    byte0 |= 0b00100000;
  }
  bytes.push_back(byte0);

  // Source
  WriteLittleEndian(bytes, source_, 4);

  // Target
  WriteLittleEndian(bytes, target_, 8);

  // Reserved 6 bytes
  for (int idx = 0; idx < 6; ++idx) {
    bytes.push_back(0);
  }

  // Ack and Res
  bytes.push_back(static_cast<uint8_t>(ack_required_ << 1)
                  | static_cast<uint8_t>(res_required_));

  // Sequence
  bytes.push_back(sequence_);

  // Reserved 8 bytes
  for (int idx = 0; idx < 8; ++idx) {
    bytes.push_back(0);
  }

  WriteLittleEndian(bytes, message_type_, 2);

  // Reserved 2 bytes
  for (int idx = 0; idx < 2; ++idx) {
    bytes.push_back(0);
  }

  return bytes;
}

bool HeaderContent::operator==(const HeaderContent& rhs) const {
  return (use_target_ == rhs.use_target() &&
          source_ == rhs.source() &&
          target_ == rhs.target() &&
          ack_required_ == rhs.ack_required() &&
          res_required_ == rhs.res_required() &&
          sequence_ == rhs.sequence() &&
          message_type_ == rhs.message_type());
}

bool HeaderContent::operator!=(const HeaderContent& rhs) const {
  return !operator==(rhs);
}

bool HeaderContent::use_target() const {
  return use_target_;
}

void HeaderContent::set_use_target(bool use_target) {
  use_target_ = use_target;
}

uint32_t HeaderContent::source() const {
  return source_;
}

void HeaderContent::set_source(uint32_t source) {
  source_ = source;
}

uint64_t HeaderContent::target() const {
  return target_;
}

void HeaderContent::set_target(uint64_t target) {
  target_ = target;
}

bool HeaderContent::ack_required() const {
  return ack_required_;
}

void HeaderContent::set_ack_required(bool ack_required) {
  ack_required_ = ack_required;
}

bool HeaderContent::res_required() const {
  return res_required_;
}

void HeaderContent::set_res_required(bool res_required) {
  res_required_ = res_required;
}

uint8_t HeaderContent::sequence() const {
  return sequence_;
}

void HeaderContent::set_sequence(uint8_t sequence) {
  sequence_ = sequence;
}

MessageTypes HeaderContent::message_type() const {
  return message_type_;
}

void HeaderContent::set_message_type(MessageTypes message_type) {
  message_type_ = message_type;
}

void HeaderContent::Init(std::vector<uint8_t>::const_iterator &it) {
  // Byte 0 = LSB of protocol

  uint16_t protocol = ((*(it + 1) & 0b1111) << 8) | *it;
  if (protocol != 0x0400) {
    throw std::invalid_argument("bytes has invalid format.");
  }

  // Byte 1, bit 3 = tagged
  use_target_ = static_cast<bool>((*(it + 1) & (~0b00010100)) != 0);
  it += 2;

  // Byte 2 - 5 = source
  source_ = static_cast<int32_t>(ReadLittleEndian(it, it + 4));
  it += 4;

  // Byte 6 - 13 = target
  target_ = ReadLittleEndian(it, it + 8);
  it += 8;

  // Byte 14 - 19 = reserved
  it += 6;

  uint8_t byte20 = *it;
  it++;

  // Byte 20, bit 7 = ack_required
  ack_required_ = (byte20 & 0b00000010) != 0;

  // Byte 20, bit 8 = res_required
  res_required_ = (byte20 & 0b00000001) != 0;

  // Byte 21 = sequence
  sequence_ = *it;
  it++;

  // Byte 22 - 29 = reserved
  it += 8;

  // Byte 30 - 31 = type
  message_type_ = static_cast<MessageTypes>(ReadLittleEndian(it, it + 2));
  it += 2;

  // Byte 32 - 33 = reserved
  it += 2;
}