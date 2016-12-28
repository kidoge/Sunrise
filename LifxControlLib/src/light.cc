#include "light.h"

Light::Light(const boost::asio::ip::address_v4& address) : address_(address) {
}

boost::asio::ip::address_v4 Light::address() const {
  return address_;
}