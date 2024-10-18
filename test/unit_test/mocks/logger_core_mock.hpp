#pragma once

#include "easylog.hpp"
#include "gmock/gmock.h"

// MOCK_METHOD(return_type, method_name, (arg_types...), (specifiers...));
class MockLoggerCore: public yq::base_logger_core
{
public:
	MOCK_METHOD(void, push_msg, (std::unique_ptr<yq::base_log_msg> msg), (override));
	MOCK_METHOD(void, check_fatal, (yq::log_level level), (override));
};
