#ifndef __YQ_LOGGER_HPP__
#define __YQ_LOGGER_HPP__
#include "logger_core.hpp"

#include <string_view>

namespace yq
{

template<log_level level>
class logger
{
public:
	/**
	 * std::ostream os require sync_with_stdio(true)
	 *
	 */
	logger(bool enable_time = false, bool enable_flush = false,
		   base_logger_core& logger_instance = logger_core::instance()):
		m_enable_time { enable_time },
		m_enable_flush { enable_flush },
		m_logger_instance { logger_instance }
	{
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

	/// Store origin pointer with unique_ptr.get() may cause data race
	void change_output(std::unique_ptr<base_output_handler> uptr_os)
	{
		m_logger_instance.change_output(std::move(uptr_os));
	}

	void change_output()
	{
		m_logger_instance.change_output();
	}

	void change_output_file(std::string_view sv)
	{
		auto os = std::make_unique<fstream_output_handler>(sv);
		m_logger_instance.change_output(std::move(os));
	}

private:
    template <typename... Args>
    void push_msg(std::optional<std::source_location> location,
                  std::format_string<Args...> fmt, Args&&... args)
    {
        // std::format_string may unable forward through std::make_unique()
        auto p_msg = std::make_unique<log_msg<Args...>>(
            level, m_enable_flush, get_time(), location, fmt,
            std::forward<Args>(args)...);

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
	
	bool m_enable_time;
	bool m_enable_flush;
	base_logger_core& m_logger_instance;
};

}	//namespace yq

#endif //__YQ_LOGGER_HPP__
