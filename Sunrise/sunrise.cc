// Sunrise.cpp : Defines the entry point for the console application.
//

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/asio.hpp>
#include <boost/program_options.hpp>
#include <vector>

#include <LifxControl/light.h>
#include <LifxControl/lifx_control.h>

using std::vector;

using boost::posix_time::seconds;
using boost::asio::ip::address;
using boost::asio::ip::address_v4;

namespace po = boost::program_options;

using lifx::Light;
using lifx::LifxControl;

address_v4 kSubnet(0xFFFFFF00);
address_v4 kLocalhost(0xC0A80102);

int main(int ac, char* av[]) {
	LifxControl lifx_control(kLocalhost, kSubnet);

  po::options_description desc("Allowed options");
  desc.add_options()
    ("help", "produces help message")
    ("enumerate", "detects LIFX devices on the network")
    ("power", po::value<bool>(), "set power");

  po::variables_map vm;
  po::store(po::parse_command_line(ac, av, desc), vm);
  po::notify(vm);

  if (vm.count("help")) {
    std::cout << desc << std::endl;
  }

  if (vm.count("enumerate")) {
    std::vector<Light> lights = lifx_control.Enumerate(seconds(1));
    for (auto it = lights.begin(); it != lights.end(); ++it) {
      std::cout << (*it).address() << std::endl;
    }
  }

  if (vm.count("power")) {
    std::cout << "power " << vm["power"].as<bool>() << std::endl;
  }
	return 0;
}
