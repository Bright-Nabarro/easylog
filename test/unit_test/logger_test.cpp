#include <sstream>
#include <array>
#include "mocks/logger_core_mock.hpp"
#include "gtest/gtest.h"
#include "easylog.hpp"

class core_sync: public testing::Test
{
protected:
	void SetUp() override
	{
	}

	void TearDown() override
	{
		origin = nullptr;
		logger.change_output();
	}

	template<typename... Args>
	auto TestWord(yq::log_level level, std::format_string<Args...> fmt, Args... args) -> std::string
	{
		using namespace std::chrono_literals;
		auto p_msg = std::make_unique<yq::log_msg<Args...>>(level,
			false, std::nullopt, std::nullopt, fmt, args...);
		std::mutex& mtx = core.get_output_mtx();
		core.push_msg(std::move(p_msg));
		while(true)
		{
			std::unique_lock lock { mtx };
			auto ret = origin->get_string(); 
			if (ret.empty())
			{
				lock.unlock();
				std::this_thread::sleep_for(10ms);
			}
			else
			{
				return ret;
			}
		}
	}

	template<typename... Args>
	auto TestLoggerWord(std::format_string<Args...> fmt, Args... args) -> std::string
	{
		using namespace std::chrono_literals;
		std::mutex& mtx = core.get_output_mtx();
		logger(fmt, args...);
		while(true)
		{
			std::unique_lock lock { mtx };
			auto ret = origin->get_string(); 
			if (ret.empty())
			{
				lock.unlock();
				std::this_thread::sleep_for(10ms);
			}
			else
			{
				return ret;
			}
		}
	}

	yq::logger<yq::log_level::info> logger;
    yq::stringstream_output_handler* origin = nullptr;
	inline static auto& core = yq::logger_core::instance();
};

TEST_F(core_sync, change_output)
{
	auto p_ssm = std::make_unique<yq::stringstream_output_handler>();
	origin = p_ssm.get();
	logger.change_output(std::move(p_ssm));
	auto ret = TestWord(yq::log_level::info, "hello");
	EXPECT_NE(ret.find("hello"), std::string::npos);
	ret = TestWord(yq::log_level::info, "mvvm");
}

TEST_F(core_sync, operator_ptn)
{
	auto p_ssm = std::make_unique<yq::stringstream_output_handler>();
	origin = p_ssm.get();
	logger.change_output(std::move(p_ssm));
	auto ret = TestLoggerWord("hello");
	EXPECT_NE(ret.find("hello"), std::string::npos);
	EXPECT_NE(ret.find(yq::log_level_to_string(yq::log_level::info)), std::string::npos);
}

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

