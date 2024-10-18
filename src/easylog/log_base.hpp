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
 * If passing log_level::fatal as log_level,
 * the logger::log function will throw this exception
 */
class log_fatal_error: public std::exception { };

class log_construct_error: public std::runtime_error
{
public:
	log_construct_error(std::string_view sv):
		runtime_error { sv.data() }
	{}
};

}	//namespace yq

#endif	//__YQ_LOGGER_BASE_HPP__

