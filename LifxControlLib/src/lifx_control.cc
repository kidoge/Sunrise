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

std::vector<uint8_t> LifxControl::CreatePacket() {
  shared_ptr<HeaderContent> header = std::make_shared<HeaderContent>();
  header->set_use_target(true);
  header->set_source(source_);
  header->set_res_required(true);
  header->set_message_type(lifx::kGetVersion);
  Packet packet(header, nullptr);
  return packet.getBytes();
}

LifxControl::LifxControl(const address_v4& localhost_addr,
                         const address_v4& subnet_mask) :
                            localhost_addr_(localhost_addr),
                            subnet_mask_(subnet_mask) {
  source_ = static_cast<uint32_t>(rand());
  io_service_ = std::make_shared<boost::asio::io_service>();
  socket_ = shared_ptr<udp::socket>(
    new udp::socket(*io_service_, udp::endpoint(udp::v4(), 56700)));
}

LifxControl::~LifxControl() {
  io_service_->stop();
}

std::vector<Light> LifxControl::Enumerate(const time_duration& timeout) {

  std::vector<uint8_t> requestMessage = CreatePacket();

  address_v4 broadcast_addr = address_v4::broadcast(localhost_addr_, subnet_mask_);
  std::shared_ptr<udp::endpoint> receiver_ptr(new udp::endpoint(broadcast_addr, 56700));

  socket_->async_send_to(boost::asio::buffer(requestMessage),
    *receiver_ptr,
    boost::bind(&LifxControl::HandleSend, this,
      boost::asio::placeholders::error,
      boost::asio::placeholders::bytes_transferred));

  io_service_->run();
  io_service_->reset();

  boost::asio::deadline_timer t(*io_service_, timeout);
  t.async_wait(boost::bind(&LifxControl::StopListening, this));
  StartReceive();
  io_service_->run();
  return std::vector<Light>(lights_);
}

Light LifxControl::CreateLight(boost::asio::ip::address_v4& light_addr) {
  return Light(io_service_, socket_, light_addr);
}

void LifxControl::AddUniqueLight(const Light& light) {
  // TODO: Just use set?
  for (auto it = lights_.begin(); it != lights_.end(); ++it) {
    if (*it == light) {
      return;
    }
  }

  lights_.push_back(light);
}

void LifxControl::HandleSend(const boost::system::error_code& ec,
                                 std::size_t bytes_transferred) {
  // Do nothing...
}

void LifxControl::HandleReceive(std::shared_ptr<udp::endpoint> sender_ptr,
                                std::shared_ptr<std::array<uint8_t, 128> > buffer,
                                const boost::system::error_code& ec,
                                std::size_t bytes_transferred) {
  // Broadcasting will send the packet to the sender as well. We should ignore it.
  if (sender_ptr->address().to_v4() != localhost_addr_) {

    Packet p(std::vector<uint8_t>(buffer->begin(), buffer->begin() + bytes_transferred));
    if (p.header().source() == source_) {

      std::vector<uint8_t> payload = p.payload();

      lock_.lock();
      AddUniqueLight(Light(io_service_, socket_, sender_ptr->address().to_v4()));
      lock_.unlock();
    }
  }
  StartReceive();
}

void LifxControl::StartReceive() {
  auto data = std::make_shared<std::array<uint8_t, 128> >();
  auto sender_ptr = std::make_shared<udp::endpoint>();
  socket_->async_receive_from(boost::asio::buffer(*data),
                             *sender_ptr,
                             boost::bind(&LifxControl::HandleReceive, this,
                                         sender_ptr,
                                         data,
                                         boost::asio::placeholders::error,
                                         boost::asio::placeholders::bytes_transferred));
}

void LifxControl::StopListening() {
  io_service_->stop();
}
