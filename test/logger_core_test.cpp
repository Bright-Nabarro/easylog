#include "mocks/log_msg_mock.hpp"

//#define MK_UPTR_MSG_OS(os) std::make_unique<MockLogMsg>(yq::log_level::info, false, os)

TEST(logger_core_test, push_msg)
{
	auto& core = yq::logger_core::instance();
	{
		std::stringstream ssm;
		auto msg = MK_UPTR_MSG_OS(ssm);
		EXPECT_CALL(*msg, log_string())
			.Times(1)
			.WillOnce(::testing::Return("0987push_msg1"));
		
		
		core.push_msg(std::move(msg));
	}
}

