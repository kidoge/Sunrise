#include <boost/asio.hpp>
#include <memory>

#include "CppUnitTest.h"

#include <LifxControl/Packet.h>
#include <LifxControl/HeaderContent.h>
#include <LifxControl/Light.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace LifxControlLibTests {		
	TEST_CLASS(HeaderContentTests) {
	public:

		TEST_METHOD(TestHeader) {
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
			header.useTarget = true;
			header.source = 0x432100;
			header.target = 0x1234;
			header.ack_required = 0;
			header.res_required = 1;
			header.sequence = 0;
			header.messageType = 0xF00D;
			
			std::vector<uint8_t> headerBytes = header.getBytes();
			Assert::AreEqual(headerBytes.size(), sizeof(expectedBytes));
			for (int idx = 0; idx < headerBytes.size(); ++idx) {
				Assert::AreEqual(headerBytes[idx], expectedBytes[idx]);
			}
		}
	};
}