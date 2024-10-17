#include "gtest/gtest.h"
#include "mocks/log_msg_mock.hpp"
#include "mocks/output_handler_mock.hpp"

class chg_core_to_sstream: public testing::Test
{
protected:
	void SetUp() override
	{
		
	}

	void TearDown() override
	{
	}

	void TestWord(std::string_view sv)
	{
		std::unique_ptr<yq::stringstream_output_handler> handler;
		yq::stringstream_output_handler* origin = handler.get();
		using namespace std::chrono_literals;
		std::mutex& mtx = core.get_output_mtx();
		while(true)
		{
			std::unique_lock lock { mtx };
			if (origin->get_string().empty())
			{
				lock.unlock();
				std::this_thread::sleep_for(10ms);
			}
			else
			{
				EXPECT_EQ(sv, origin->get_string());
				break;
			}
		}
	}

	inline static auto& core = yq::logger_core::instance();
};

inline static auto& core = yq::logger_core::instance();
TEST(logger_core_test, push_msg)
{
	auto handler = std::make_unique<yq::stringstream_output_handler>();
	auto* origin = handler.get();
	core.change_output(std::move(handler));
	yq::log_msg msg { yq::log_level::info, false,
		std::nullopt, std::nullopt, "hello" };
	
	core.push_msg(
		std::unique_ptr<decltype(msg)>(
			std::make_unique<decltype(msg)>(msg)
		)
	);
	
	using namespace std::chrono_literals;
	std::mutex& mtx = core.get_output_mtx();
	while(true)
	{
		std::unique_lock lock { mtx };
		if (origin->get_string().empty())
		{
			lock.unlock();
			std::this_thread::sleep_for(10ms);
		}
		else
		{
			std::cout<<"World: "<<origin->get_string()<<std::endl;
			break;
		}
	}
	
}

