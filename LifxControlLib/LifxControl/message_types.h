#pragma once

#include <memory>

namespace lifx {
  enum ServiceTypes {
    kServiceUDP = 1,
  };

  enum MultiZoneRequestTypes {
    kNoApply = 0,
    kApply = 1,
    kApplyOnly = 2,
  };

  enum MessageTypes {
    kNullMessage = 0,

    // Device messages
    kGetService = 2,
    kStateService = 3,
    kGetHostInfo = 12,
    kStateHostInfo = 13,
    kGetHostFirmware = 14,
    kStateHostFirmware = 15,
    kGetWifiInfo = 16,
    kStateWifiInfo = 17,
    kGetWifiFirmware = 18,
    kStateWifiFirmware = 19,
    kGetDevicePower = 20,
    kSetDevicePower = 21,
    kStateDevicePower = 22,
    kGetLabel = 23,
    kSetLabel = 24,
    kStateLabel = 25,
    kGetVersion = 32,
    kStateVersion = 33,
    kGetInfo = 34,
    kStateInfo = 35,
    kAcknowledgement = 45,
    kGetLocation = 48,
    kStateLocation = 50,
    kGetGroup = 51,
    kStateGroup = 53,
    kEchoRequest = 58,
    kEchoResponse = 59,

    // Light messages
    kGetLightState = 101,
    kSetColor = 102,
    kState = 107,
    kGetLedPower = 116,
    kSetLedPower = 117,
    kStateLedPower = 118,
    kGetInfrared = 121,
    kStateInfrared = 122,

    // MultiZone messages
    kSetColorZones = 501,
    kGetColorZones = 502,
    kStateZone = 503,
    kStateMultiZone = 506,

  };
}