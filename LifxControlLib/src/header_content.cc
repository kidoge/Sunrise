#include <exception>

#include "header_content.h"

using lifx::MessageTypes;
using lifx::HeaderContent;

uint64_t readLittleEndian(std::vector<uint8_t>::iterator begin_it, 
                          std::vector<uint8_t>::iterator end_it) {
  std::vector<uint8_t>::iterator it = end_it - 1;
  uint64_t result = 0;
  while (it >= begin_it) {
    result = result << 8 | (*it);
    it--;
  }
  return result;
}

HeaderContent::HeaderContent(std::vector<uint8_t> bytes) {
  // Byte 0 = LSB of protocol

  uint16_t protocol = ((bytes[1] & 0b1111) << 8) | bytes[0];
  if (protocol != 0x0400) {
    throw std::invalid_argument("bytes has invalid format.");
  }

  // Byte 1, bit 3 = tagged
  use_target_ = static_cast<bool>((bytes[1] & (~0b00010100)) != 0);
  
  // Byte 2 - 5 = source
  source_ = static_cast<int32_t>(readLittleEndian(bytes.begin() + 2, 
                                                  bytes.begin() + 6));
  
  // Byte 6 - 13 = target
  target_ = readLittleEndian(bytes.begin() + 6, bytes.begin() + 14);

  // Byte 14 - 19 = reserved
  for (int idx = 14; idx <= 19; ++idx) {
    if (bytes[idx] != 0) {
      throw std::invalid_argument("bytes has invalid format.");
    }
  }

  uint8_t byte20 = bytes[20];

  // Byte 20, bit 7 = ack_required
  ack_required_ = (byte20 & 0b00000010) != 0;

  // Byte 20, bit 8 = res_required
  res_required_ = (byte20 & 0b00000001) != 0;

  // Byte 21 = sequence
  sequence_ = static_cast<uint8_t>(bytes[21]);

  // Byte 22 - 29 = reserved
  for (int idx = 22; idx <= 29; ++idx) {
    if (bytes[idx] != 0) {
      throw std::invalid_argument("bytes has invalid format.");
    }
  }
  // Byte 30 - 31 = type
  message_type_ = static_cast<MessageTypes>(
    readLittleEndian(bytes.begin() + 30, bytes.begin() + 32));

  // Byte 32 - 33 = reserved
  for (int idx = 32; idx <= 33; ++idx) {
    if (bytes[idx] != 0) {
      throw std::invalid_argument("bytes has invalid format.");
    }
  }
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
  bytes.push_back(static_cast<uint8_t>(source_ & 0x000000FF));
  bytes.push_back(static_cast<uint8_t>((source_ & 0x0000FF00) >> 8));
  bytes.push_back(static_cast<uint8_t>((source_ & 0x00FF0000) >> 16));
  bytes.push_back(static_cast<uint8_t>((source_ & 0xFF000000) >> 24));

  // Target
  bytes.push_back(static_cast<uint8_t>(target_ & 0x00000000000000FF));
  bytes.push_back(static_cast<uint8_t>((target_ & 0x000000000000FF00) >> 8));
  bytes.push_back(static_cast<uint8_t>((target_ & 0x0000000000FF0000) >> 16));
  bytes.push_back(static_cast<uint8_t>((target_ & 0x00000000FF000000) >> 24));
  bytes.push_back(static_cast<uint8_t>((target_ & 0x000000FF00000000) >> 32));
  bytes.push_back(static_cast<uint8_t>((target_ & 0x0000FF0000000000) >> 40));
  bytes.push_back(static_cast<uint8_t>((target_ & 0x00FF000000000000) >> 48));
  bytes.push_back(static_cast<uint8_t>((target_ & 0xFF00000000000000) >> 56));

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

  bytes.push_back(message_type_ & 0x00FF);
  bytes.push_back((message_type_ & 0xFF00) >> 8);

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