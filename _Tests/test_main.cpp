//Google test entry point
#include <gtest/gtest.h>

//NOTE: Include all tests under the gtest so we do not have to include it in every test
#include "base64_test.h"
#include "JsonFormatter_test.h"

int main(int argc, char* argv[])
{
	testing::InitGoogleTest(&argc,argv);
	return RUN_ALL_TESTS();
}