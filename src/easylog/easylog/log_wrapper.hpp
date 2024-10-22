#ifndef __YQ_LOG_WRAPPER_HPP__
#define __YQ_LOG_WRAPPER_HPP__

#include "logger.hpp"

namespace yq
{

template<log_level level>
struct simple_log_invoker
{
	simple_log_invoker() = default;
	template<typename... Args>
	void operator()(std::format_string<Args...> fmt, Args&& ... args)
	{
		m_log(fmt, std::forward<Args>(args)...);
	}

	template<typename... Args>
	void operator()(const std::source_location& location,
			std::format_string<Args...> fmt, Args&& ... args)
	{
		m_log(location, fmt, std::forward<Args>(args)...);
	}
	
	logger<level> m_log;
};

[[nodiscard]] consteval
auto loc(const std::source_location& location = std::source_location::current())
	-> std::source_location
{
	return location;
}

template<typename... Args>
void info(std::format_string<Args...> fmt, Args&& ... args)
{
	simple_log_invoker<log_level::info>{}(fmt, std::forward<Args>(args)...);
}

template<typename... Args>
void info(const std::source_location& location, std::format_string<Args...> fmt, Args&& ... args)
{
	simple_log_invoker<log_level::info>{}(location, fmt, std::forward<Args>(args)...);
}

template<typename... Args>
void warn(std::format_string<Args...> fmt, Args&& ... args)
{
	simple_log_invoker<log_level::warn>{}(fmt, std::forward<Args>(args)...);
}

template<typename... Args>
void warn(const std::source_location& location, std::format_string<Args...> fmt, Args&& ... args)
{
	simple_log_invoker<log_level::warn>{}(location, fmt, std::forward<Args>(args)...);
}

template<typename... Args>
void debug(std::format_string<Args...> fmt, Args&& ... args)
{
	simple_log_invoker<log_level::debug>{}(fmt, std::forward<Args>(args)...);
}

template<typename... Args>
void debug(const std::source_location& location, std::format_string<Args...> fmt, Args&& ... args)
{
	simple_log_invoker<log_level::debug>{}(location, fmt, std::forward<Args>(args)...);
}

template<typename... Args>
void error(std::format_string<Args...> fmt, Args&& ... args)
{
	simple_log_invoker<log_level::error>{}(fmt, std::forward<Args>(args)...);
}

template<typename... Args>
void error(const std::source_location& location, std::format_string<Args...> fmt, Args&& ... args)
{
	simple_log_invoker<log_level::error>{}(location, fmt, std::forward<Args>(args)...);
}

template<typename... Args>
void fatal(std::format_string<Args...> fmt, Args&& ... args)
{
	simple_log_invoker<log_level::fatal>{}(fmt, std::forward<Args>(args)...);
}

template<typename... Args>
void fatal(const std::source_location& location, std::format_string<Args...> fmt, Args&& ... args)
{
	simple_log_invoker<log_level::fatal>{}(location, fmt, std::forward<Args>(args)...);
}

}	//namespace yq

#endif 	//__YQ_LOG_WRAPPER_HPP__
