#pragma once

#include <memory>
#include <vector>

#include "message_types.h"

struct HeaderContent {
public:
  std::vector<uint8_t> GetBytes();

  bool use_target() const;

  void set_use_target(bool use_target);

  uint32_t source() const;

  void set_source(uint32_t source);

  uint64_t target() const;

  void set_target(uint64_t target);

  bool ack_required() const;

  void set_ack_required(bool ack_required);

  bool res_required() const;

  void set_res_required(bool res_required);

  uint8_t sequence() const;

  void set_sequence(uint8_t sequence);

  lifx::MessageTypes message_type() const;

  void set_message_type(lifx::MessageTypes message_type);

private:
  /* frame */
  bool use_target_ = false; // (tagged in docs) use frame address target field
  uint32_t source_ = 0; // ID for distinguishing clients

  /* frame address */
  uint64_t target_ = 0; // 6 byte MAC address, or 0 for all devices
  bool ack_required_ = false; // device returns acknowledgement packet
  bool res_required_ = false; // device returns response packet
  uint8_t sequence_ = 0; // ID for distinguishing responses for same client

  /* protocol header */
  lifx::MessageTypes message_type_ = lifx::kNullMessage; // Type of message
};