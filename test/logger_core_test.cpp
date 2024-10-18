#include "gtest/gtest.h"
#include "mocks/log_msg_mock.hpp"
#include "mocks/output_handler_mock.hpp"

class chg_core_to_sstream: public testing::Test
{
protected:
	void SetUp() override
	{
		auto handler = std::make_unique<yq::stringstream_output_handler>();
		origin = handler.get();
		core.change_output(std::move(handler));
	}

	void TearDown() override
	{
		origin = nullptr;
		core.change_output();
	}

	auto TestWord(std::unique_ptr<yq::base_log_msg> p_msg) -> std::string
	{
		using namespace std::chrono_literals;
		std::mutex& mtx = core.get_output_mtx();
		core.push_msg(std::move(p_msg));
		while(true)
		{
			std::unique_lock lock { mtx };
			if (auto ret = origin->get_string(); ret.empty())
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

#define TEST_WORD(str)                                                         \
    TestWord(std::make_unique<yq::log_msg<>>(yq::log_msg<>(                    \
        yq::log_level::info, false, std::nullopt, std::nullopt, str)))

    yq::stringstream_output_handler* origin;
	inline static auto& core = yq::logger_core::instance();
};

TEST_F(chg_core_to_sstream, log_msg)
{
	auto mock_msg = std::make_unique<MockLogMsg>(yq::log_level::info, false); 
	EXPECT_CALL(*mock_msg, log_string())
		.Times(1)
		.WillOnce(testing::Return("return"));
	EXPECT_CALL(*mock_msg, get_level)
		.Times(testing::AtMost(1));
	EXPECT_CALL(*mock_msg, enable_flush)
		.Times(1);
	auto ret = TestWord(std::move(mock_msg));
	EXPECT_NE(ret.find("return"), std::string::npos);
}

TEST_F(chg_core_to_sstream, push_msg_str)
{
	auto hello_str = TEST_WORD("hello");
	EXPECT_NE(hello_str.find("hello"), std::string::npos);

	auto hello_str2 = TEST_WORD("123321");
	EXPECT_NE(hello_str2.find("123321"), std::string::npos);

	auto ciallo_str = TEST_WORD("Ciallo～(∠・ω< )⌒★");
	EXPECT_NE(ciallo_str.find("Ciallo～(∠・ω< )⌒★"), std::string::npos);
}

TEST_F(chg_core_to_sstream, check_fatal)
{
	EXPECT_THROW(core.check_fatal(yq::log_level::fatal), yq::log_fatal_error);
}


class chg_core_to_mock_sstream: public chg_core_to_sstream
{
protected:
	void SetUp() override
	{
		auto handler = std::make_unique<mock_stringstream_output_handler>();
		p_handler = handler.get();
		origin = handler.get();
		core.change_output(std::move(handler));
	}

	void TestMockWord(std::unique_ptr<yq::base_log_msg> p_msg)
	{
		EXPECT_CALL(*p_handler, write)
			.Times(1);
		EXPECT_CALL(*p_handler, get_string)
			.WillOnce(testing::Return("mock_return"));
		auto ret = TestWord(std::move(p_msg));
		EXPECT_EQ(ret, "mock_return");
	}

	mock_stringstream_output_handler* p_handler;
};


TEST_F(chg_core_to_mock_sstream, log_msg)
{
	auto mock_msg = std::make_unique<MockLogMsg>(yq::log_level::info, false); 
	EXPECT_CALL(*mock_msg, log_string())
		.Times(1)
		.WillOnce(testing::Return("return"));
	EXPECT_CALL(*mock_msg, get_level)
		.Times(testing::AtMost(1));
	EXPECT_CALL(*mock_msg, enable_flush)
		.Times(1);
	
	TestMockWord(std::move(mock_msg));
}


