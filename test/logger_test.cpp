#include "gtest/gtest.h"
#include "easylog.hpp"

TEST(logger_test, logger_test_base)
{
	yq::logger::instance().log(yq::log_level::info, "hello world");
}
