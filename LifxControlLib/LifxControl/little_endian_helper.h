#include <vector>

namespace lifx {

  uint64_t ReadLittleEndian(std::vector<uint8_t>::const_iterator begin_it,
                            std::vector<uint8_t>::const_iterator end_it);

  void WriteLittleEndian(std::vector<uint8_t>& bytes, uint64_t number, size_t size);
}
