#include <sstream>

#include "gtest/gtest.h"
#include "easylog.hpp"

class test_log_msg: public yq::base_log_msg
{
public:
	test_log_msg(yq::log_level level, bool enable_flush = false,
		std::optional<std::reference_wrapper<std::ostream>> os = std::nullopt):
		base_log_msg{ level, enable_flush, os }
	{}
	virtual auto log_string() const -> std::string
	{
		return "";
	};
};

class BaseLogTestFixture: public testing::Test
{
protected:
	void SetUp() override
	{

	}

	void TearDown() override
	{
	}
};

TEST(base_log_msg_test, base_log_msg)
{
	{
		std::ostringstream ossm;
		test_log_msg log_msg { yq::log_level::info, false, ossm };
		EXPECT_EQ(&log_msg.get_os()->get(), &ossm);
		EXPECT_EQ(log_msg.enable_flush(), false);
		EXPECT_EQ(log_msg.get_level(), yq::log_level::info);
	}
	{
		std::stringstream ssm;
		test_log_msg log_msg { yq::log_level::info, false, ssm };
		EXPECT_EQ(&log_msg.get_os()->get(), &ssm);
		EXPECT_EQ(log_msg.enable_flush(), false);
		EXPECT_EQ(log_msg.get_level(), yq::log_level::info);

	}
}

#define INIT_LOG_MSG_OS(msg_name, os)                                          \
    yq::log_msg msg_name                                                       \
    {                                                                          \
        yq::log_level::info, false, os, std::nullopt, yq::loc(), "hello",      \
    }

TEST(log_msg_test, log_msg_base_method)
{
    {
        std::ostringstream ossm;
        INIT_LOG_MSG_OS(msg, ossm);
        EXPECT_EQ(&msg.get_os()->get(), &ossm);
        EXPECT_EQ(msg.enable_flush(), false);
        EXPECT_EQ(msg.get_level(), yq::log_level::info);
    }
    {
        std::ostringstream ssm;
        INIT_LOG_MSG_OS(msg, ssm);
        EXPECT_EQ(&msg.get_os()->get(), &ssm);
        EXPECT_EQ(msg.enable_flush(), false);
        EXPECT_EQ(msg.get_level(), yq::log_level::info);
    }
}

// Additional parameters are required
#define INIT_LOG_MSG_OS_STR(msg_name, os, fmt, ...)                            \
    yq::log_msg msg_name                                                       \
    {                                                                          \
        yq::log_level::info, false, os, std::nullopt, yq::loc(), fmt,          \
            __VA_ARGS__                                                        \
    }

TEST(log_msg_test, log_msg_log_string)
{
    {
		std::ostringstream ossm;
		INIT_LOG_MSG_OS(msg, ossm);
		auto text = msg.log_string();
		EXPECT_NE(text.find("hello"), std::string::npos);
		EXPECT_NE(text.find(yq::log_level_to_string(yq::log_level::info)), std::string::npos);
		EXPECT_EQ(text.find("Time"), std::string::npos);
	}
	{
		std::stringstream ssm;
		INIT_LOG_MSG_OS_STR(msg, ssm, "hello {}", "world");
		EXPECT_NE(msg.log_string().find("hello world"), std::string::npos);
	}
	{
		std::stringstream ssm;
		INIT_LOG_MSG_OS_STR(msg, ssm, "没想到竟会有这样的{1}造访{0}", "轻音部", "危机");
		auto str = msg.log_string();
		EXPECT_NE(str.find("没想到竟会有这样的危机造访轻音部"), std::string::npos);
		EXPECT_NE(str.find(yq::log_level_to_string(yq::log_level::info)), std::string::npos);
	}
}

