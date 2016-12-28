#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/array.hpp>

#include "Light.h"
#include "LightEnumerator.h"

using boost::posix_time::time_duration;
using boost::asio::ip::address_v4;
using boost::asio::ip::udp;

size_t createGreen(boost::array<char, 128>& buffer) {
  // Origin Indicator
  char bytes[]{ /*message size*/0x31, 0x00,
    /*origin indicator, tagged, addressable, 4 bits of protocol*/0x00, 
    /*rest of protocol*/0x34,
    /*source*/0x00, 0x12, 0x34, 0x00, 0x00, 0x00,
    /*target*/0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    /*reserved*/0x00, 0x00, 0x00, 0x00,
    /*ack, res*/0x03,
    /*sequence number*/0x00,
    /*reserved*/0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    /*message type*/0x66, 0x00,
    /*reserved*/0x00, 0x00,
    /*Reserve (Payload start)*/0x00,
    /*HSB*/0x55, 0x55, 0xFF,
    /*saturation*/0xFF,
    /*brightness*/0x01, 0x00,
    /*kelvin*/0xAC, 0x0D,
    /*transition*/ 0x00, 0x08, 0x00, 0x00};
  char *c0 = buffer.data();
  char *c1 = bytes;
  for (int i = 0; i < 49; ++i) {
    *(c0++) = *(c1++);
  }
  return 49;
}

LightEnumerator::LightEnumerator(boost::asio::io_service& service, 
                                 const address_v4& localhost_addr_, 
                                 const address_v4& subnet_mask_) : 
                                   localhost_addr(localhost_addr_), 
                                   subnet_mask(subnet_mask_) {
  std::cout << "localhost: " << localhost_addr.to_string() << std::endl << "subnet: " << subnet_mask.to_string() << std::endl;

  udp::socket socket(service, udp::endpoint(udp::v4(), 56700));
  boost::array<char, 128> reqMsg;
  size_t len = createGreen(reqMsg);

  // TODO: Send a packet to every IP
  address_v4 broadcast_addr = address_v4::broadcast(localhost_addr, subnet_mask);
  std::shared_ptr<udp::endpoint> receiver_ptr(new udp::endpoint(broadcast_addr, 56700));
  
  socket.async_send_to(boost::asio::buffer(reqMsg, len), 
                       *receiver_ptr, 
                       boost::bind(&LightEnumerator::handleSend, this, 
                                   boost::asio::placeholders::error, 
                                   boost::asio::placeholders::bytes_transferred));

  service.run();
  service.reset();

  boost::asio::deadline_timer t(service, boost::posix_time::seconds(5));
  t.async_wait(boost::bind(&LightEnumerator::stop_listening, this, boost::ref(service)));
  start_receive(socket);
  service.run();
}

std::vector<Light> LightEnumerator::get_lights(const time_duration& timeout) const {
  return std::vector<Light>();
}

void LightEnumerator::handleSend(const boost::system::error_code& ec,
                                 std::size_t bytes_transferred) {
  // Do nothing...
}

void LightEnumerator::handleReceive(udp::socket& socket, 
                                    std::shared_ptr<udp::endpoint> sender_ptr,
                                    std::shared_ptr<std::array<unsigned char, 128> > buffer,
                                    const boost::system::error_code& ec,
                                    std::size_t bytes_transferred) {
  std::cout << sender_ptr->address() << std::endl;
  if (sender_ptr->address().to_v4() != localhost_addr) {
    std::cout << sender_ptr->address().to_string() << " responded." << std::endl;
    for (std::size_t idx = 0; idx < bytes_transferred; ++idx) {
      std::cout << std::to_string((*buffer)[idx]) << " ";
    }
    std::cout << "(len: " << bytes_transferred << ")" << std::endl;
    lock.lock();
    lights.push_back(Light(sender_ptr->address().to_v4()));
    lock.unlock();
  } else {
    std::cout << "broadcast to self" << std::endl;
  }
  start_receive(socket);
}

void LightEnumerator::start_receive(udp::socket& socket) {
  auto data = std::make_shared<std::array<unsigned char, 128> >();
  auto sender_ptr = std::make_shared<udp::endpoint>();
  socket.async_receive_from(boost::asio::buffer(*data), 
                            *sender_ptr, 
                            boost::bind(&LightEnumerator::handleReceive, this, 
                                        boost::ref(socket),
                                        sender_ptr,
                                        data,
                                        boost::asio::placeholders::error, 
                                        boost::asio::placeholders::bytes_transferred));
}

void LightEnumerator::stop_listening(boost::asio::io_service& service) {
  service.stop();
}
