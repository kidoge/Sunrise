#pragma once

#include <memory>
#include <boost/asio.hpp>

namespace lifx {
  class Light {

  public:
    Light(std::shared_ptr<boost::asio::io_service> service,
          std::shared_ptr<boost::asio::ip::udp::socket> sock,
          const boost::asio::ip::address_v4& addr);

    boost::asio::ip::address_v4 address() const;

    bool operator== (const Light& rhs) const;
    bool operator!= (const Light& rhs) const;

    void SetPower(bool power);

    void SetHSBK(double h, double s, double b, uint16_t k, double duration);

  private:
    std::shared_ptr<boost::asio::io_service> service_;
    std::shared_ptr<boost::asio::ip::udp::socket> socket_;
    boost::asio::ip::address_v4 addr_;
    
  };
}
