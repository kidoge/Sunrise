#include "light.h"

using lifx::Light;
Light::Light(std::shared_ptr<boost::asio::io_service> service,
             std::shared_ptr<boost::asio::ip::udp::socket> sock,
             const boost::asio::ip::address_v4& addr)
  : service_(service), socket_(sock), addr_(addr) {

}

boost::asio::ip::address_v4 Light::address() const {
  return addr_;
}