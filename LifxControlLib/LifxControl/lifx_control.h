#pragma once

#include <boost/asio.hpp>

#include <vector>
#include <mutex>
#include <memory>

namespace lifx {
  class Light;
  class io_service;

  class LifxControl {

  public:
    LifxControl(const boost::asio::ip::address_v4& localhost_addr,
                const boost::asio::ip::address_v4& subnet_mask);

    ~LifxControl();
    
    std::vector<Light> Enumerate(const boost::posix_time::time_duration& timeout);

    Light CreateLight(boost::asio::ip::address_v4& light_addr);

  private:
    std::shared_ptr<boost::asio::io_service> io_service_;
    std::shared_ptr<boost::asio::ip::udp::socket> socket_;
    std::vector<Light> lights_;
    std::mutex lock_;
    boost::asio::ip::address_v4 localhost_addr_;
    boost::asio::ip::address_v4 subnet_mask_;

    void HandleSend(const boost::system::error_code& ec,
                    std::size_t bytes_transferred);

    void HandleReceive(std::shared_ptr<boost::asio::ip::udp::endpoint> sender_ptr,
                       std::shared_ptr<std::array<unsigned char, 128> > buffer,
                       const boost::system::error_code& ec,
                       std::size_t bytes_transferred);

    void StartReceive();

    void StopListening();
  };
}