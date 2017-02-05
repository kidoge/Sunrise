#include <boost/asio.hpp>
#include <exception>
#include <memory>

#include "CppUnitTest.h"

#include <LifxControl/light.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

using lifx::Light;
using boost::asio::ip::address_v4;

namespace LifxControlLibTests {
  TEST_CLASS(HeaderContentTests) {
	public:
    TEST_METHOD(TestEqualAddressEquality) {
      //TODO: Be able to mock io_service and socket
      auto service = std::make_shared<boost::asio::io_service>();
      auto socket = std::shared_ptr<boost::asio::ip::udp::socket>(new boost::asio::ip::udp::socket(*service));
      Light light1(service, socket, address_v4(1));
      Light light2(service, socket, address_v4(1));

      Assert::IsTrue(light1 == light2);
    }

    TEST_METHOD(TestDifferentAddressEquality) {
      auto service = std::make_shared<boost::asio::io_service>();
      auto socket = std::shared_ptr<boost::asio::ip::udp::socket>(new boost::asio::ip::udp::socket(*service));
      Light light1(service, socket, address_v4(1));
      Light light2(service, socket, address_v4(2));

      Assert::IsTrue(light1 != light2);
    }
  };
}