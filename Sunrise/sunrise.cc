// Sunrise.cpp : Defines the entry point for the console application.
//

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/asio.hpp>
#include <vector>

#include <LifxControl/light.h>
#include <LifxControl/lifx_control.h>

using std::vector;

using boost::posix_time::seconds;
using boost::asio::ip::address;
using boost::asio::ip::address_v4;


using lifx::Light;
using lifx::LifxControl;

address_v4 kSubnet(0xFFFFFF00);
address_v4 kLocalhost(0xC0A80102);

int main() {
	LifxControl lifx_control(kLocalhost, kSubnet);
	vector<Light> lights = lifx_control.Enumerate(seconds(1));
	
  for (auto it = lights.begin(); it != lights.end(); ++it) {
    it->SetPower(1);
    std::cout << (*it).address() << std::endl;
  }
	std::cout << "Total lights: " << lights.size() << std::endl;

	std::cout << "Done" << std::endl;
  std::cout.flush();
	getchar();
	return 0;
}
