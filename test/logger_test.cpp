#include <sstream>
#include <array>
#include "mocks/logger_core_mock.hpp"
#include "gtest/gtest.h"
#include "easylog.hpp"

//TEST(logger_test, logger_basic_use)
//{
//	std::stringstream ssm;
//	yq::logger<yq::log_level::info>{ssm}("hello");
//	yq::logger<yq::log_level::info>{ssm}("hello world");
//	yq::logger<yq::log_level::info>{ssm}(yq::loc(), "hello {}", "Bright Nabarro");
//
//	std::string output = ssm.str();
//	EXPECT_TRUE(output.find("hello") != std::string::npos);
//	EXPECT_TRUE(output.find("hello world") != std::string::npos);
//	EXPECT_TRUE(output.find("hello Bright Nabarro") != std::string::npos);
//}

//TEST(logger_test, logger_logger_core)
//{
//	MockLoggerCore mock_logger_core;
//	constexpr auto info = yq::log_level::info;
//	yq::logger<info> test_logger_info { mock_logger_core };
//
//	EXPECT_CALL(mock_logger_core, push_msg(::testing::_)).Times(1);
//	EXPECT_CALL(mock_logger_core, check_fatal(info)).Times(1);
//	
//	constexpr auto error = yq::log_level::error;
//	yq::logger<error> test_logger_error { mock_logger_core };
//
//	EXPECT_CALL(mock_logger_core, push_msg(::testing::_)).Times(1);
//	EXPECT_CALL(mock_logger_core, check_fatal(error)).Times(1);
//}

