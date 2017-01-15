#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/array.hpp>

#include "light.h"
#include "lifx_control.h"
#include "packet.h"
#include "header_content.h"
#include "message_types.h"

using boost::posix_time::time_duration;
using boost::asio::ip::address_v4;
using boost::asio::ip::udp;

using lifx::LifxControl;
using lifx::Packet;
using lifx::HeaderContent;
using lifx::Light;

using std::shared_ptr;

std::vector<uint8_t> CreateGreen() {
  // Origin Indicator

  shared_ptr<HeaderContent> header = std::make_shared<HeaderContent>();
  header->set_use_target(true);
  header->set_source(0x1);
  header->set_res_required(true);
  header->set_message_type(lifx::kSetColor);

  uint8_t payload_data[] {
    /*Reserve (Payload start)*/0x00,
    /*HSB*/0x55, 0x55, 0xFF,
    /*saturation*/0xFF,
    /*brightness*/0x01, 0x00,
    /*kelvin*/0xAC, 0x0D,
    /*transition*/ 0x00, 0x04, 0x00, 0x00};
  shared_ptr<std::vector<uint8_t> > payload = std::make_shared<std::vector<uint8_t> >();
  payload->reserve(sizeof(payload_data));
  payload->assign(payload_data, payload_data + sizeof(payload_data));
  Packet packet(header, payload);
  return packet.getBytes();
}

LifxControl::LifxControl(boost::asio::io_service& service,
                                 const address_v4& localhost_addr,
                                 const address_v4& subnet_mask) :
                                   localhost_addr_(localhost_addr),
                                   subnet_mask_(subnet_mask) {
  std::cout << "localhost: " << localhost_addr_.to_string() << std::endl
    << "subnet: " << subnet_mask_.to_string() << std::endl;

  udp::socket socket(service, udp::endpoint(udp::v4(), 56700));

  std::vector<uint8_t> requestMessage = CreateGreen();

  // TODO: Send a packet to every IP
  address_v4 broadcast_addr = address_v4::broadcast(localhost_addr_, subnet_mask_);
  std::shared_ptr<udp::endpoint> receiver_ptr(new udp::endpoint(broadcast_addr, 56700));

  socket.async_send_to(boost::asio::buffer(requestMessage),
                       *receiver_ptr,
                       boost::bind(&LifxControl::HandleSend, this,
                                   boost::asio::placeholders::error,
                                   boost::asio::placeholders::bytes_transferred));

  service.run();
  service.reset();

  boost::asio::deadline_timer t(service, boost::posix_time::seconds(5));
  t.async_wait(boost::bind(&LifxControl::StopListening, this, boost::ref(service)));
  StartReceive(socket);
  service.run();
}

std::vector<Light> LifxControl::GetLights(const time_duration& timeout) const {
  return std::vector<Light>();
}

void LifxControl::HandleSend(const boost::system::error_code& ec,
                                 std::size_t bytes_transferred) {
  // Do nothing...
}

void LifxControl::HandleReceive(udp::socket& socket,
                                    std::shared_ptr<udp::endpoint> sender_ptr,
                                    std::shared_ptr<std::array<uint8_t, 128> > buffer,
                                    const boost::system::error_code& ec,
                                    std::size_t bytes_transferred) {
  std::cout << sender_ptr->address() << std::endl;
  if (sender_ptr->address().to_v4() != localhost_addr_) {
    std::cout << sender_ptr->address().to_string() << " responded." << std::endl;
    for (std::size_t idx = 0; idx < bytes_transferred; ++idx) {
      std::cout << std::to_string((*buffer)[idx]) << " ";
    }
    std::cout << "(len: " << bytes_transferred << ")" << std::endl;
    lock_.lock();
    lights_.push_back(Light(sender_ptr->address().to_v4()));
    lock_.unlock();
  } else {
    std::cout << "broadcast to self" << std::endl;
  }
  StartReceive(socket);
}

void LifxControl::StartReceive(udp::socket& socket) {
  auto data = std::make_shared<std::array<uint8_t, 128> >();
  auto sender_ptr = std::make_shared<udp::endpoint>();
  socket.async_receive_from(boost::asio::buffer(*data),
                            *sender_ptr,
                            boost::bind(&LifxControl::HandleReceive, this,
                                        boost::ref(socket),
                                        sender_ptr,
                                        data,
                                        boost::asio::placeholders::error,
                                        boost::asio::placeholders::bytes_transferred));
}

void LifxControl::StopListening(boost::asio::io_service& service) {
  service.stop();
}
