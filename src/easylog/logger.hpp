#ifndef __YQ_LOGGER_HPP__
#define __YQ_LOGGER_HPP__
#include "logger_core.hpp"

namespace yq
{

template<log_level level>
class logger
{
public:
	logger(std::optional<std::reference_wrapper<std::ostream>> os = std::nullopt,
		   bool enable_time = false, bool enable_flush = false,
		   base_logger_core& logger_instance = logger_core::instance()):
		m_os { os },
		m_enable_time { enable_time },
		m_enable_flush { enable_flush },
		m_logger_instance { logger_instance }
	{
		if (m_os.has_value() && !m_os->get())
			throw log_construct_error { std::format("Logger stream error") };
	}
  
	template<typename... Args>
	void operator()(std::format_string<Args...> fmt, Args&& ... args)
	{
		if constexpr (!enable_log)
			return;
		if constexpr (!enable_debug)
			return;

		push_msg(std::nullopt, fmt, std::forward<Args>(args)...);
	}

	template<typename... Args>
	void operator()(std::source_location location, std::format_string<Args...> fmt, Args&& ... args)
	{
		if constexpr (!enable_log)
			return;
		if constexpr (!enable_debug)
			return;

		push_msg(std::move(location), fmt, std::forward<Args>(args)...);
	}
	
	void set_enable_time(bool enable)
	{
		m_enable_time = enable;
	}

	void set_enable_flush(bool enable)
	{
		m_enable_flush = enable;
	}

	void set_output_stream(std::ostream& os)
	{
		m_os = os;
	}

private:
	template<typename... Args>
	void push_msg(std::optional<std::source_location> location, std::format_string<Args...> fmt, Args&& ... args)
	{
		// std::format_string may unable forward through std::make_unique()
		std::unique_ptr<base_log_msg> p_msg {
			new log_msg<Args...>(level, m_enable_flush, m_os,
					get_time(), location, fmt, std::forward<Args>(args)...)
		};
		m_logger_instance.push_msg(std::move(p_msg));
		m_logger_instance.check_fatal(level);
	}

	static constexpr bool enable_debug =
	#ifdef NDEBUG
		false
	#else
		true
	#endif
	;

	static constexpr bool enable_log =
	#ifdef NO_YQ_LOG
		false
	#else
		true
	#endif
	;

private:
	auto get_time() const -> std::optional<std::chrono::system_clock::time_point>
	{
		if (m_enable_time)
		{
			return std::chrono::system_clock::now();
		}
		return std::nullopt;
	}
	
	std::optional<std::reference_wrapper<std::ostream>> m_os;
	bool m_enable_time;
	bool m_enable_flush;
	base_logger_core& m_logger_instance;
};


}	//namespace yq

#endif //__YQ_LOGGER_HPP__
