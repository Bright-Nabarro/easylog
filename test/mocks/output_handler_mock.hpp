#pragma once

#include "easylog.hpp"
#include "gmock/gmock.h"

// MOCK_METHOD(return_type, method_name, (arg_types...), (specifiers...));
class mock_stringstream_output_handler:
	public yq::stringstream_output_handler
{
public:
	MOCK_METHOD(void, write, (std::string_view), (override));
};


