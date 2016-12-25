#include "stdafx.h"
#include "CppUnitTest.h"
#include "Light.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest {		
	TEST_CLASS(UnitTest1) {
	public:
		TEST_METHOD(TestMethodPass) {
			// TODO: Your test code here
			Assert::AreEqual(1, 1);
		}
	};
}