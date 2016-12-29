#include <boost/asio.hpp>
#include <memory>

#include "CppUnitTest.h"

#include <LifxControl/packet.h>
#include <LifxControl/header_content.h>
#include <LifxControl/light.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

using lifx::HeaderContent;

namespace LifxControlLibTests {		
	TEST_CLASS(HeaderContentTests) {
	public:

		TEST_METHOD(TestFullHeader) {
			uint8_t expectedBytes[] {
				/*origin indicator, tagged, addressable, protocol*/ 0x00, 0x34,
				/*source*/ 0x00, 0x21, 0x43, 0x00,
				/*target*/ 0x34, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				/*reserved*/ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				/*ack, res*/ 0x01,
				/*sequence number*/ 0x00,
				/*reserved*/ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				/*message type*/ 0x0D, 0xF0,
				/*reserved*/ 0x00, 0x00,
			};

			HeaderContent header;
      header.set_use_target(true);
      header.set_source(0x432100);
			header.set_target(0x1234);
			header.set_res_required(true);
			header.set_message_type(static_cast<lifx::MessageTypes>(0xF00D));
			
			std::vector<uint8_t> headerBytes = header.GetBytes();
			Assert::AreEqual(headerBytes.size(), sizeof(expectedBytes));
			for (int idx = 0; idx < headerBytes.size(); ++idx) {
				Assert::AreEqual(headerBytes[idx], expectedBytes[idx]);
			}
		}
	};
}