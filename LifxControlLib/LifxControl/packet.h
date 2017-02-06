#pragma once

#include <memory>
#include <vector>

namespace lifx {
  class HeaderContent;

  class Packet {
  public:
    Packet(const std::vector<uint8_t>& bytes);
    Packet(std::shared_ptr<HeaderContent>header,
           std::shared_ptr<std::vector<uint8_t> > payload);

    std::vector<uint8_t> getBytes();

    HeaderContent header();
    std::vector<uint8_t> payload();
  private:
    std::shared_ptr<HeaderContent> header_;
    std::shared_ptr<std::vector<uint8_t> > payload_;
  };
}