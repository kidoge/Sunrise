// Sunrise.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/array.hpp>
#include <boost/asio.hpp>

#include <cstdint>
#include <iostream>
#include <vector>
#include <mutex>
#include <shared_mutex>
#include <memory>

#include "Sunrise.h"

using std::uint8_t;
using std::uint32_t;
using std::string;
using std::vector;
using std::shared_ptr;
using std::mutex;
using std::shared_lock;
using std::array;
using std::ostringstream;

using boost::posix_time::time_duration;
using boost::posix_time::seconds;
using boost::asio::io_service;
using boost::asio::ip::udp;
using boost::asio::ip::address;
using boost::asio::ip::address_v4;

// TODO: Namespace

address_v4 kSubnet(0xFFFFFF00);
address_v4 kLocalhost(0xC0A80102);

size_t createGreen(boost::array<char, 128>& buffer) {
	// Origin Indicator
	char bytes[] { /*message size*/0x31, 0x00, 
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
		/*brightness*/0x40, 0x00, 
		/*kelvin*/0xAC, 0x0D,
		/*transition*/ 0x00, 0x08, 0x00, 0x00 };
	char *c0 = buffer.data();
	char *c1 = bytes;
	for (int i = 0; i < 49; ++i) {
		*(c0++) = *(c1++);
	}
	return 49;
}

class Light {
	address addr;

public:
	Light(const address& addr_) : addr(addr_) {}
};

class LightEnumerator {
	vector<Light> lights;
	mutex lock;

public:
	LightEnumerator(io_service& service, time_duration timeout) {
		std::cout << "localhost: " << kLocalhost.to_string() << std::endl << "subnet: " << kSubnet.to_string() << std::endl;

		udp::resolver resolver(service);

		udp::socket socket(service, udp::endpoint(udp::v4(), 56700));
		udp::endpoint local_endpoint = udp::endpoint(kLocalhost, 56700);
		boost::array<char, 128> reqMsg;
		size_t len = createGreen(reqMsg);

		// TODO: Send a packet to every IP
		address_v4 broadcastIP = address_v4::broadcast(kLocalhost, kSubnet);

		udp::resolver::query query(udp::v4(), broadcastIP.to_string(), "56700");
		shared_ptr<udp::endpoint> receiver_ptr(new udp::endpoint(*resolver.resolve(query)));

		socket.async_send_to(boost::asio::buffer(reqMsg, len),
			*receiver_ptr,
			boost::bind(&LightEnumerator::listenForResponse,
				this,
				receiver_ptr,
				boost::ref(socket),
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));

		boost::asio::deadline_timer t(service, boost::posix_time::seconds(5));
		service.run();
	}

	void listenForResponse(shared_ptr<udp::endpoint> receiver_ptr, 
			udp::socket& socket,
			const boost::system::error_code& ec,
			std::size_t bytes_transferred) {
		std::cout << receiver_ptr->address().to_string() << " sent. Listening..." <<std::endl;
		std::shared_ptr<udp::endpoint> sender_ptr = std::make_shared<udp::endpoint>();
		std::shared_ptr<array<unsigned char,128> > buffer(new array<unsigned char,128>());
		socket.async_receive_from(boost::asio::buffer(*buffer), 
				*receiver_ptr,
				boost::bind(&LightEnumerator::handleResponse,
				this,
					receiver_ptr,
					buffer,
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred));
	}

	void handleResponse(shared_ptr<udp::endpoint> sender_ptr,
			shared_ptr<array<unsigned char,128> > buffer,
			const boost::system::error_code& ec,
			std::size_t bytes_transferred) {
		if (sender_ptr->address().to_v4() != kLocalhost) {
			std::cout << sender_ptr->address().to_string() << " responded." << std::endl;
			for (std::size_t idx = 0; idx < bytes_transferred; ++idx) {
				std::cout << std::to_string((*buffer)[idx]) << " ";
			}
			std::cout << "(len: " << bytes_transferred << ")" << std::endl;
			lock.lock();
			lights.push_back(Light(sender_ptr->address()));
			lock.unlock();
		}
		else {
			std::cout << "broadcast to self" << std::endl;
		}
	}

	// Return a copy of the vector
	vector<Light> getLights() const {
		return lights;
	}
};

int main() {
	boost::asio::io_service service;
	LightEnumerator enumerator(service, seconds(1));
	vector<Light> lights = enumerator.getLights();
	
	std::cout << "Total lights: " << lights.size() << std::endl;

	std::cout << "Done" << std::endl;
	getchar();
	return 0;
}
