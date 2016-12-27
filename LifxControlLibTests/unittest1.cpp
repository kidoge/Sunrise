#include <boost/asio.hpp>

#include "CppUnitTest.h"

#include <LifxControl/Packet.h>
#include <LifxControl/Light.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace LifxControlLibTests
{		
	TEST_CLASS(UnitTest1)
	{
	public:
		
		TEST_METHOD(TestMethod1)
		{
			boost::asio::ip::address_v4 addr;
			Light l(addr);
			Assert::AreEqual(1, 1);
		}
	};
}