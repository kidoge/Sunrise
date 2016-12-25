// Sunrise.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"


#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/asio.hpp>
#include <vector>

#include "Light.h"
#include "LightEnumerator.h"

using std::vector;

using boost::posix_time::seconds;
using boost::asio::ip::address;
using boost::asio::ip::address_v4;

// TODO: Namespace

address_v4 kSubnet(0xFFFFFF00);
address_v4 kLocalhost(0xC0A8007F);

int main() {
	boost::asio::io_service service;
	LightEnumerator enumerator(service, kLocalhost, kSubnet);
	vector<Light> lights = enumerator.get_lights(seconds(5));
	
	std::cout << "Total lights: " << lights.size() << std::endl;

	std::cout << "Done" << std::endl;
	getchar();
	return 0;
}
