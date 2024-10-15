#include "gtest/gtest.h"
#include "easylog.hpp"

TEST(logger_test, logger_test_base)
{
	yq::info("hello");
	//yq::info_loc("hello");
	yq::logger<yq::log_level::info>{}("hello world");
	yq::info("hello {}", "Bright Nabarro");
}
