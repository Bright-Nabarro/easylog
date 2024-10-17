#include "gtest/gtest.h"
#include "easylog.hpp"

#include <iostream>
#include <sstream>

class rdbuf_cout_to_ssm: public testing::Test
{
protected:
	void SetUp() override
	{
		origin = std::cout.rdbuf(ossm.rdbuf());
	}

	void TearDown() override
	{
		std::cout.rdbuf(origin);
		origin = nullptr;
	}

	std::ostringstream ossm;
	std::streambuf* origin = nullptr;
};

TEST_F(rdbuf_cout_to_ssm, stream_output_handler_write)
{
	yq::stream_output_handler<std::ostream> out;
	out.write("hello world");
	EXPECT_EQ(ossm.str(), "hello world");
}

TEST_F(RDBUF_COUT_TO_SSM, STREAM_OUTPUT_HANDLER_OTHER)
{
}
