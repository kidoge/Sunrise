#pragma once

#include <boost/asio.hpp>

class Light {

public:
  Light(const boost::asio::ip::address_v4& address);

  boost::asio::ip::address_v4 address() const;
private:
  boost::asio::ip::address_v4 address_;

};
