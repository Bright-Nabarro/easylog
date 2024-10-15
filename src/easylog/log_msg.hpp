#ifndef __YQ_LOG_MSG_HPP__
#define __YQ_LOG_MSG_HPP__

#include <tuple>
#include <optional>
#include <format>
#include <source_location>
#include <chrono>

#include "log_base.hpp"

namespace yq
{


class base_log_msg
{
public:
    base_log_msg(log_level level, bool enable_flush = false,
                 std::optional<std::reference_wrapper<std::ostream>> os = {})
        : m_level{level}, m_enable_flush{enable_flush}, m_os{os}
    { }

    virtual ~base_log_msg() = default;
	virtual auto log_string() const -> std::string = 0;
	virtual auto get_level() const -> log_level
	{
		return m_level;
	}
	virtual auto get_os() const -> std::optional<std::reference_wrapper<std::ostream>>
	{
		return m_os;
	}
	virtual auto enable_flush() const -> bool
	{
		return m_enable_flush;
	}
private:
	log_level m_level;
	bool m_enable_flush;
	std::optional<std::reference_wrapper<std::ostream>> m_os;
};

template<typename... Args>
class log_msg: public base_log_msg
{	
public:
	log_msg(log_level level,
			bool enable_flush,
			std::optional<std::reference_wrapper<std::ostream>> os,
			std::optional<std::chrono::system_clock::time_point> time,
			std::optional<std::source_location> location,
			std::format_string<Args...> fmt,
			Args&& ... args):
		base_log_msg { level, enable_flush, os },
		m_time { time },
		m_location { location },
		m_fmt { fmt },
		m_args { std::make_tuple(std::forward<Args>(args)...) }
	{ }
	
	auto log_string() const -> std::string override
	{
		std::stringstream ssm;
		//log level
		ssm<<std::format("{: <12} ", std::format("[{}]", log_level_to_string(get_level())));
		
		//time
		if (m_time.has_value())
		{
			std::time_t log_time = std::chrono::system_clock::to_time_t(*m_time);
			ssm<<std::put_time(std::localtime(&log_time), "%Y-%m-%d %H:%M:%S")<<' ';
		}

		//location
		if (m_location.has_value())
		{
			ssm<<std::format("[File: {}] ", m_location->file_name());
			ssm<<std::format("[Function: {}] ", m_location->function_name());
			ssm<<std::format("[Line: {}] ", m_location->line());
		}

		constexpr auto args_size = std::tuple_size_v<decltype(m_args)>;
		ssm<<log_string_helper(std::make_index_sequence<args_size>{});

		return ssm.str();
	}

private:

	template<std::size_t... Indices>
	auto log_string_helper(std::index_sequence<Indices...>) const -> std::string
	{
		return std::vformat(
			m_fmt.get(),
			std::make_format_args(std::get<Indices>(m_args)...)
		);
	}

	const std::optional<std::chrono::system_clock::time_point> m_time;
	const std::optional<std::source_location> m_location;
	const std::format_string<Args...> m_fmt;
	const std::tuple<std::decay_t<Args>...> m_args;
};

}	//namespace yq

#endif

