#include <boost/asio.hpp>
#include <exception>
#include <memory>

#include "CppUnitTest.h"

#include <LifxControl/packet.h>
#include <LifxControl/header_content.h>
#include <LifxControl/light.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

using lifx::HeaderContent;

const std::vector<uint8_t> kDefaultPacket {
  /*origin indicator, tagged, addressable, protocol*/ 0x00, 0x14,
  /*source*/ 0x00, 0x00, 0x00, 0x00,
  /*target*/ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /*reserved*/ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /*ack, res*/ 0x00,
  /*sequence number*/ 0x00,
  /*reserved*/ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /*message type*/ 0x00, 0x00,
  /*reserved*/ 0x00, 0x00,
};

namespace LifxControlLibTests {		
	TEST_CLASS(HeaderContentTests) {
	public:
    TEST_METHOD(TestDefaultHeader) {
      HeaderContent header;

      compareResult(header.GetBytes(), kDefaultPacket);
    }

    TEST_METHOD(TestUseTargetField) {
      std::vector<uint8_t> expectedBytes{
        /*origin indicator, tagged, addressable, protocol*/ 0x00, 0x34,
        /*source*/ 0x00, 0x00, 0x00, 0x00,
        /*target*/ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        /*reserved*/ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        /*ack, res*/ 0x00,
        /*sequence number*/ 0x00,
        /*reserved*/ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        /*message type*/ 0x00, 0x00,
        /*reserved*/ 0x00, 0x00,
      };

      HeaderContent header;
      header.set_use_target(true);

      compareResult(header.GetBytes(), expectedBytes);
    }

    TEST_METHOD(TestSourceField) {
      std::vector<uint8_t> expectedBytes{
        /*origin indicator, tagged, addressable, protocol*/ 0x00, 0x14,
        /*source*/ 0x21, 0x43, 0x65, 0x87,
        /*target*/ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        /*reserved*/ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        /*ack, res*/ 0x00,
        /*sequence number*/ 0x00,
        /*reserved*/ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        /*message type*/ 0x00, 0x00,
        /*reserved*/ 0x00, 0x00,
      };

      HeaderContent header;
      header.set_source(0x87654321);

      compareResult(header.GetBytes(), expectedBytes);
    }

    TEST_METHOD(TestTarget) {
      std::vector<uint8_t> expectedBytes{
        /*origin indicator, tagged, addressable, protocol*/ 0x00, 0x14,
        /*source*/ 0x00, 0x00, 0x00, 0x00,
        /*target*/ 0xBC, 0x9A, 0x78, 0x56, 0x34, 0x12, 0x00, 0x00,
        /*reserved*/ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        /*ack, res*/ 0x00,
        /*sequence number*/ 0x00,
        /*reserved*/ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        /*message type*/ 0x00, 0x00,
        /*reserved*/ 0x00, 0x00,
      };

      HeaderContent header;
      header.set_target(0x123456789ABC);

      compareResult(header.GetBytes(), expectedBytes);
    }

    TEST_METHOD(TestAckField) {
      std::vector<uint8_t> expectedBytes{
        /*origin indicator, tagged, addressable, protocol*/ 0x00, 0x14,
        /*source*/ 0x00, 0x00, 0x00, 0x00,
        /*target*/ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        /*reserved*/ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        /*ack, res*/ 0x02,
        /*sequence number*/ 0x00,
        /*reserved*/ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        /*message type*/ 0x00, 0x00,
        /*reserved*/ 0x00, 0x00,
      };

      HeaderContent header;
      header.set_ack_required(true);

      compareResult(header.GetBytes(), expectedBytes);
    }

    TEST_METHOD(TestResField) {
      std::vector<uint8_t> expectedBytes{
        /*origin indicator, tagged, addressable, protocol*/ 0x00, 0x14,
        /*source*/ 0x00, 0x00, 0x00, 0x00,
        /*target*/ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        /*reserved*/ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        /*ack, res*/ 0x01,
        /*sequence number*/ 0x00,
        /*reserved*/ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        /*message type*/ 0x00, 0x00,
        /*reserved*/ 0x00, 0x00,
      };

      HeaderContent header;
      header.set_res_required(true);

      compareResult(header.GetBytes(), expectedBytes);
    }

    TEST_METHOD(TestMessageTypeField) {
      std::vector<uint8_t> expectedBytes{
        /*origin indicator, tagged, addressable, protocol*/ 0x00, 0x14,
        /*source*/ 0x00, 0x00, 0x00, 0x00,
        /*target*/ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        /*reserved*/ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        /*ack, res*/ 0x00,
        /*sequence number*/ 0x00,
        /*reserved*/ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        /*message type*/ 0x57, 0x13,
        /*reserved*/ 0x00, 0x00,
      };

      HeaderContent header;
      header.set_message_type(static_cast<lifx::MessageTypes>(0x1357));

      compareResult(header.GetBytes(), expectedBytes);
    }

		TEST_METHOD(TestFullHeader) {
			std::vector<uint8_t> expectedBytes {
				/*origin indicator, tagged, addressable, protocol*/ 0x00, 0x34,
        /*source*/ 0x21, 0x43, 0x65, 0x87,
        /*target*/ 0xBC, 0x9A, 0x78, 0x56, 0x34, 0x12, 0x00, 0x00,
				/*reserved*/ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        /*ack, res*/ 0x03,
				/*sequence number*/ 0x00,
				/*reserved*/ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        /*message type*/ 0x57, 0x13,
				/*reserved*/ 0x00, 0x00,
			};

			HeaderContent header;
      header.set_use_target(true);
      header.set_source(0x87654321);
      header.set_target(0x123456789ABC);
      header.set_ack_required(true);
			header.set_res_required(true);
			header.set_message_type(static_cast<lifx::MessageTypes>(0x1357));
			
      compareResult(header.GetBytes(), expectedBytes);
		}

    void compareResult(const std::vector<uint8_t> &output, 
                       const std::vector<uint8_t> &expected) {
      Assert::AreEqual(output.size(), expected.size());
      for (int idx = 0; idx < output.size(); ++idx) {
        Assert::AreEqual(output[idx], expected[idx]);
      }
    }

    TEST_METHOD(TestConstructorWithIncorrectProtocol) {
      std::vector<uint8_t> bytes = kDefaultPacket;
      bytes[0] = 0xFF;

      Assert::ExpectException<std::invalid_argument>([bytes] {
        HeaderContent testHeader(bytes);
      });

      bytes[0] = 0;
      bytes[1] |= 0b00001111;

      Assert::ExpectException<std::invalid_argument>([bytes] {
        HeaderContent testHeader(bytes);
      });
    }

    TEST_METHOD(TestConstructorWithIncorrectReserved) {
      std::vector<uint8_t> bytes = kDefaultPacket;
      std::vector<bool> shouldBeZero {
        /*origin indicator, tagged, addressable, protocol*/ true, true,
        /*source*/ false, false, false, false,
        /*target*/ false, false, false, false, false, false, false, false,
        /*reserved*/ true, true, true, true, true, true,
        /*ack, res*/ false,
        /*sequence number*/ false,
        /*reserved*/ true, true, true, true, true, true, true, true,
        /*message type*/ false, false,
        /*reserved*/ true, true
      };

      for (int idx = 0; idx < bytes.size(); ++idx) {
        bytes[idx] = 0xFF;
        if (shouldBeZero[idx]) {
          Assert::ExpectException<std::invalid_argument>([bytes] {
            HeaderContent testHeader(bytes);
          });
        } else {
          // If this throws an exception, test will fail.
          HeaderContent testHeader(bytes);
        }
        bytes[idx] = kDefaultPacket[idx];
      }
    }
  };
}