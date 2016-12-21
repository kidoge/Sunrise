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

// TODO: Namespace

std::string formIPv4String(uint32_t address) {
	uint8_t n1 = (address >> 24) & 0xFF;
	uint8_t n2 = (address >> 16) & 0xFF;
	uint8_t n3 = (address >> 8) & 0xFF;
	uint8_t n4 = address & 0xFF;
	ostringstream os;
	os << n1 << "." << n2 << "." << n3 << "." << n4;

	return os.str();
}

size_t createGreen(boost::array<char, 128>& buffer) {
	// Origin Indicator
	char bytes[] { 0x31, 0x00, 0x00, 0x34, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*ack, res*/0x03, /*sequence number*/0x00, /*reserved*/0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*message type*/0x66, 0x00, /*reserved*/0x00, 0x00, /*Reserve (Payload start)*/0x00, /*HSB*/0x55, 0x55, 0xFF, /*saturation*/0xFF, /*brightness*/0xFF, 0xFF, /*kelvin*/0xAC, 0x0D,/*transition*/ 0x00, 0x08, 0x00, 0x00 };
	char *c0 = buffer.data();
	char *c1 = bytes;
	for (int i = 0; i < 49; ++i) {
		*(c0++) = *(c1++);
	}
	return 49;
}

size_t createStatus(boost::array<char, 128>& buffer) {
	// Origin Indicator
	char bytes[]{ 0x31, 0x00, 0x00, 0x34, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x66, 0x00, 0x00, 0x00, 0x00, 0x55, 0x55, 0xFF, 0xFF, 0xFF, 0xFF, 0xAC, 0x0D, 0x00, 0x04, 0x00, 0x00 };
	char *c0 = buffer.data();
	char *c1 = bytes;
	for (int i = 0; i < 49; ++i) {
		*(c0++) = *(c1++);
	}
	return 49;
}

class Light {
	string hostname;

public:
	Light(const string& hostname_) : hostname(hostname_) {}
};

class LightEnumerator {
	shared_ptr<vector<Light>> lights;
	mutex lock;

public:
	LightEnumerator(io_service& service, time_duration timeout) {
		lights = shared_ptr<vector<Light>>(new vector<Light>());

		udp::resolver resolver(service);

		udp::socket socket(service, udp::endpoint(udp::v4(), 56700));
		udp::endpoint local_endpoint = udp::endpoint(boost::asio::ip::address::from_string("192.168.1.2"), 56700);
		boost::array<char, 128> reqMsg;
		size_t len = createGreen(reqMsg);

		// TODO: Do this for each IP address in subnet
		string hostname = "192.168.1.6";
		udp::resolver::query query(udp::v4(), hostname, "56700");
		udp::endpoint endpoint = *resolver.resolve(query);

		socket.async_send_to(boost::asio::buffer(reqMsg, len), 
				endpoint, 
				boost::bind(&LightEnumerator::listenForResponse, 
					this,
					boost::ref(hostname),
					boost::ref(socket),
					boost::ref(endpoint),
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred));

		service.run();
	}

	void listenForResponse(const string& hostname, 
			udp::socket& socket,
			udp::endpoint& endpoint,
			const boost::system::error_code& ec,
			std::size_t bytes_transferred) {
		std::cout << hostname << " sent. Listening..." <<std::endl;
		std::shared_ptr<array<char,128> > buffer(new array<char,128>());
		socket.async_receive_from(boost::asio::buffer(*buffer), 
				endpoint,
				boost::bind(&LightEnumerator::handleResponse,
				this,
					boost::ref(hostname),
					buffer,
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred));
	}

	void handleResponse(const string& hostname,
			std::shared_ptr<array<char,128> > buffer,
			const boost::system::error_code& ec,
			std::size_t bytes_transferred) {
		std::cout << hostname << " responded." << std::endl;
		std::cout << buffer << std::endl;

	}

	const shared_ptr<vector<Light>> getLights() const {
		return lights;
	}
};

int main() {
	boost::asio::io_service service;
	LightEnumerator enumerator(service, seconds(1));
	shared_ptr<vector<Light>> lights = enumerator.getLights();
	
	std::cout << "Total lights: " << lights->size() << std::endl;

	std::cout << "Done" << std::endl;
	getchar();
	return 0;
}
