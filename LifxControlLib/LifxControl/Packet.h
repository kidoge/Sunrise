#pragma once

#include <vector>

class Packet {
public:
  std::vector<unsigned char> getBytes();

private:

  /* variable length payload follows */
};