#include "little_endian_helper.h"

uint64_t lifx::ReadLittleEndian(std::vector<uint8_t>::const_iterator begin_it,
                          std::vector<uint8_t>::const_iterator end_it) {
  auto it = end_it - 1;
  uint64_t result = 0;
  while (it >= begin_it) {
    result = result << 8 | (*it);
    it--;
  }
  return result;
}

void lifx::WriteLittleEndian(std::vector<uint8_t>& bytes, uint64_t number, size_t size) {
  for (size_t byte_idx = 0; byte_idx < size; ++byte_idx) {
    bytes.push_back(number & 0xFF);
    number = number >> 8;
  }
}
