// Sunrise.cpp : Defines the entry point for the console application.
//

#include <regex>
#include <string>
#include <vector>

#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/program_options.hpp>

#include <LifxControl/lifx_control.h>
#include <LifxControl/light.h>

using std::vector;

using boost::asio::ip::address;
using boost::asio::ip::address_v4;
using boost::lexical_cast;
using boost::posix_time::seconds;

namespace po = boost::program_options;

using lifx::LifxControl;
using lifx::Light;

const std::string kArgRequiredMsg = " argument is required.";
const std::string kIPArgInvalidMsg = " argument is invalid. It should be of format \'###.###.###.###\'.";
const std::string kColorArgInvalidMsg = " argument is invalid. It should be followed by four numbers between 0 and 1.";

uint32_t str2ip(const std::string& str) {
  uint32_t addr;
  std::regex ip_regex("([0-9]+)\\.([0-9]+)\\.([0-9]+)\\.([0-9]+)");
  std::smatch tokens;

  std::regex_match(str, tokens, ip_regex);
  if (tokens.size() != 5) {
    throw std::runtime_error("Invalid number of tokens!");
  }
  for (size_t idx = 1; idx < tokens.size(); ++idx) {
    int value = lexical_cast<int>(tokens[idx].str());
    if (value > 255 || value < 0) {
      throw std::runtime_error("Invalid token value!");
    }
    addr = addr << 8 | value;
  }
  return addr;
}

int main(int ac, char* av[]) {
  std::string subnetArg;
  std::string localhostArg;

  address_v4 subnet;
  address_v4 localhost;

  po::options_description desc("Generic options");
  desc.add_options()
    ("help", "produces help message")
    ("enumerate", "detects LIFX devices on the network")
    ("power", po::value<bool>(), "set power")
    ("color", po::value<vector<double>>(), "set color")
    ("local_ip", po::value<std::string>(), "specify the localhost IP address")
    ("subnet", po::value<std::string>(), "specify the subnet");

  po::variables_map vm;
  try {
    po::store(po::parse_command_line(ac, av, desc), vm);
    po::notify(vm);
  } catch (const std::exception& e) {
    std::cerr << "error: " << e.what() << std::endl;;
    return 1;
  } catch (...) {
    std::cerr << "Exception of unknown type!" << std::endl;
    return 1;
  }

  if (vm.count("help")) {
    std::cout << desc << std::endl;
    return 0;
  }

  if (vm.count("local_ip")) {
    try {
      localhost = address_v4(str2ip(vm["local_ip"].as<std::string>()));
    } catch (const std::exception& e) {
      std::cerr << "local_ip" << kIPArgInvalidMsg << std::endl;
      std::cerr << e.what() << std::endl;
      return 1;
    }
  } else {
    std::cerr << "local_ip" << kArgRequiredMsg << std::endl;
    return 1;
  }

  if (vm.count("subnet")) {
    try {
      subnet = address_v4(str2ip(vm["subnet"].as<std::string>()));
    } catch (const std::exception& e) {
      std::cerr << "subnet" << kIPArgInvalidMsg << std::endl;
      std::cerr << e.what() << std::endl;
      return 1;
    }
  } else {
    std::cerr << "subnet" << kArgRequiredMsg << std::endl;
    return 1;
  }

  LifxControl lifx_control(localhost, subnet);

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
