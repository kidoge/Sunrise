#include "light.h"
#include "header_content.h"
#include "packet.h"
#include "little_endian_helper.h"

using lifx::Light;
using lifx::Packet;
using boost::asio::ip::udp;

Light::Light(std::shared_ptr<boost::asio::io_service> service,
             std::shared_ptr<boost::asio::ip::udp::socket> sock,
             const boost::asio::ip::address_v4& addr)
  : service_(service), socket_(sock), addr_(addr) {

}

boost::asio::ip::address_v4 Light::address() const {
  return addr_;
}

bool Light::operator== (const Light& rhs) const {
  return addr_ == rhs.address();
}

bool Light::operator!= (const Light& rhs) const {
  return !operator==(rhs);
}

void Light::SetPower(bool power) {
  auto hc = std::make_shared<HeaderContent>();
  hc->set_message_type(kSetDevicePower);

  auto payload = std::make_shared<std::vector<uint8_t> >();
  if (power) {
    WriteLittleEndian(*payload, 0xFFFF, 2);
  } else {
    WriteLittleEndian(*payload, 0, 2);
  }

  Packet packet(hc, payload);
  std::vector<uint8_t> bytes = packet.getBytes();
  size_t p = socket_->send_to(boost::asio::buffer(bytes), udp::endpoint(addr_, 56700));
}

void Light::SetHSBK(double h, double s, double b, uint16_t k, double duration) {
  auto hc = std::make_shared<HeaderContent>();
  hc->set_message_type(kSetColor);

  auto payload = std::make_shared<std::vector<uint8_t> >();
  payload->push_back(0); //Reserved
  WriteLittleEndian(*payload, static_cast<uint64_t>(h * 0xFFFF), 2);
  WriteLittleEndian(*payload, static_cast<uint64_t>(s * 0xFFFF), 2);
  WriteLittleEndian(*payload, static_cast<uint64_t>(b * 0xFFFF), 2);
  WriteLittleEndian(*payload, static_cast<uint64_t>(k), 2);
  WriteLittleEndian(*payload, static_cast<uint64_t>(duration * 1000), 4);

  Packet packet(hc, payload);
  std::vector<uint8_t> bytes = packet.getBytes();
  size_t p = socket_->send_to(boost::asio::buffer(bytes), udp::endpoint(addr_, 56700));
}