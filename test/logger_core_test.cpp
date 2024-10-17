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

	inline static auto& core = yq::logger_core::instance();
};

inline static auto& core = yq::logger_core::instance();
TEST(logger_core_test, push_msg)
{
	auto handler = std::make_unique<yq::stringstream_output_handler>();
	auto* origin = handler.get();
	core.change_output(std::move(handler));
	core.push_msg(
		std::unique_ptr<yq::log_msg>(
			yq::log_level::
}

