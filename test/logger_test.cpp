#include <sstream>
#include "gtest/gtest.h"
#include "easylog.hpp"

TEST(logger_test, logger_test_basic)
{
	std::stringstream ssm;
	auto* original = std::cout.rdbuf();
	std::cout.rdbuf(ssm.rdbuf());

	yq::info("hello");
	yq::logger<yq::log_level::info>{}("hello world");
	yq::info(yq::loc(), "hello {}", "Bright Nabarro");

	std::cout.rdbuf(original);
	std::string output = ssm.str();
	EXPECT_TRUE(output.find("hello") != std::string::npos);
	EXPECT_TRUE(output.find("hello world") != std::string::npos);
	EXPECT_TRUE(output.find("hello Bright Nabarro") != std::string::npos);
}

TEST(logger_test, logger_test_log_output)
{

}

