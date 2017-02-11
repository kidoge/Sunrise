#include "light.h"
#include "header_content.h"
#include "packet.h"

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
    payload->push_back(255);
    payload->push_back(255);
  } else {
    payload->push_back(0);
    payload->push_back(0);
  }

  Packet packet(hc, payload);
  std::vector<uint8_t> bytes = packet.getBytes();
  size_t p = socket_->send_to(boost::asio::buffer(bytes), udp::endpoint(addr_, 56700));
}
