/**
 * @file log_base.hpp
 * @brief Contains base components: enum, exception
 */

#ifndef __YQ_LOGGER_BASE_HPP__
#define __YQ_LOGGER_BASE_HPP__
#include <stdexcept>

namespace yq
{

enum class log_level
{
    info = 0,
    warn,
    debug,
    error,
    fatal
};

[[nodiscard]] 
constexpr auto log_level_to_string(log_level level) -> const char*
{
	switch(level)
	{
	case log_level::info:
		return "Info";
	case log_level::warn:
		return "Warning";
	case log_level::debug:
		return "Debug";
	case log_level::error:
		return "Error";
	case log_level::fatal:
		return "Fatal";
	default:
		return "Unkown";
	}
}

/**
 * Do not catch `log_fatal_error`.
 * These exceptions indicate irrecoverable errors
 * and should be allowed to propagate naturally,
 * leading to program termination.
 * Catching these exceptions may lead to undefined
 * behavior or incomplete resource cleanup.
 */
class log_fatal_error
{ };

class log_construct_error: public std::runtime_error
{
public:
	log_construct_error(std::string_view sv):
		runtime_error { sv.data() }
	{}
};

template<typename Exption>
concept DescribeException = requires(const char* msg)
{
	Exption { msg };
};

template<DescribeException Exception>
void exception_handle(std::string_view msg = "") 
{
#ifdef __cpp_exceptions
	throw Exception{msg.data()};
#else
	exit(1);
#endif
}

template<typename Exception>
void exception_handle()
{
#ifdef __cpp_exceptions
	throw Exception{};
#else
	exit(1);
#endif
}


}	//namespace yq

#endif	//__YQ_LOGGER_BASE_HPP__

