#pragma once

#include <gmock/gmock.h>
#include "easylog.hpp"

// MOCK_METHOD(return_type, method_name, (arg_types...), (specifiers...));
class MockLogMsg: public yq::base_log_msg
{
public:
	using yq::base_log_msg::base_log_msg;
	MOCK_METHOD(std::string, log_string, (), (const, override));
	MOCK_METHOD(yq::log_level, get_level, (), (const, override));
	MOCK_METHOD(bool, enable_flush, (), (const, override));
};

