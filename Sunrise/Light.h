#pragma once

#include <boost/asio.hpp>

class Light {

public:
  Light(const boost::asio::ip::address_v4& addr_);

private:
  boost::asio::ip::address_v4 addr;

};
