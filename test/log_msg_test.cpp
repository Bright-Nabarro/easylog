#include <sstream>

#include "gtest/gtest.h"
#include "easylog.hpp"

class test_log_msg: public yq::base_log_msg
{
public:
	test_log_msg(yq::log_level level, bool enable_flush = false):
		base_log_msg{ level, enable_flush }
	{}
	virtual auto log_string() const -> std::string
	{
		return "";
	};
};

TEST(base_log_msg_test, base_log_msg)
{
	{
		test_log_msg log_msg { yq::log_level::info, false};
		EXPECT_EQ(log_msg.enable_flush(), false);
		EXPECT_EQ(log_msg.get_level(), yq::log_level::info);
	}
	{
		test_log_msg log_msg { yq::log_level::info, false };
		EXPECT_EQ(log_msg.enable_flush(), false);
		EXPECT_EQ(log_msg.get_level(), yq::log_level::info);

	}
}

#define INIT_LOG_MSG(msg_name)                                                 \
	yq::log_msg msg_name                                                       \
	{                                                                          \
		yq::log_level::info, false, std::nullopt, yq::loc(), "hello",          \
	}

TEST(log_msg_test, log_msg_base_method)
{
    {
        INIT_LOG_MSG(msg);
        EXPECT_EQ(msg.enable_flush(), false);
        EXPECT_EQ(msg.get_level(), yq::log_level::info);
    }
    {
        INIT_LOG_MSG(msg);
        EXPECT_EQ(msg.enable_flush(), false);
        EXPECT_EQ(msg.get_level(), yq::log_level::info);
    }
}

// Additional parameters are required
#define INIT_LOG_MSG_STR(msg_name, fmt, ...)                                   \
	yq::log_msg msg_name                                                       \
	{                                                                          \
		yq::log_level::info, false, std::nullopt, yq::loc(), fmt, __VA_ARGS__  \
	}

TEST(log_msg_test, log_msg_log_string)
{
    {
		INIT_LOG_MSG(msg);
		auto text = msg.log_string();
		EXPECT_NE(text.find("hello"), std::string::npos);
		EXPECT_NE(text.find(yq::log_level_to_string(yq::log_level::info)), std::string::npos);
		EXPECT_EQ(text.find("Time"), std::string::npos);
	}
	{
		INIT_LOG_MSG_STR(msg, "hello {}", "world");
		EXPECT_NE(msg.log_string().find("hello world"), std::string::npos);
	}
	{
		INIT_LOG_MSG_STR(msg, "没想到竟会有这样的{1}造访{0}", "轻音部", "危机");
		auto str = msg.log_string();
		EXPECT_NE(str.find("没想到竟会有这样的危机造访轻音部"), std::string::npos);
		EXPECT_NE(str.find(yq::log_level_to_string(yq::log_level::info)), std::string::npos);
	}
}

