#pragma once

#include <boost/asio.hpp>

#include <vector>
#include <mutex>
#include <memory>

class Light;
class io_service;

class LightEnumerator {

public:
  LightEnumerator(boost::asio::io_service& service,
                  const boost::asio::ip::address_v4& localhost_addr,
                  const boost::asio::ip::address_v4& subnet_mask);

  std::vector<Light> get_lights(const boost::posix_time::time_duration& timeout) const;

private:
  std::vector<Light> lights;
  std::mutex lock;
  boost::asio::ip::address_v4 localhost_addr;
  boost::asio::ip::address_v4 subnet_mask;

  void handleSend(const boost::system::error_code& ec,
                  std::size_t bytes_transferred);

  void handleReceive(std::shared_ptr<boost::asio::ip::udp::endpoint> sender_ptr,
                     std::shared_ptr<std::array<unsigned char, 128> > buffer,
                     const boost::system::error_code& ec,
                     std::size_t bytes_transferred);

  void start_receive();
};