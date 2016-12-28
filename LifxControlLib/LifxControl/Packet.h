#pragma once

#include <memory>
#include <vector>

class HeaderContent;

class Packet {
public:
  std::vector<uint8_t> getBytes();
  Packet(std::shared_ptr<HeaderContent>header_ptr, std::shared_ptr<std::vector<uint8_t> > payload_ptr);

private:
	std::shared_ptr<HeaderContent> header;
	std::shared_ptr<std::vector<uint8_t> > payload;
};