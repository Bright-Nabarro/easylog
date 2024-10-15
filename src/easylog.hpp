#ifndef __EASYLOG_HPP__
#define __EASYLOG_HPP__

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <format>
#include <iostream>
#include <memory>
#include <optional>
#include <queue>
#include <source_location>
#include <stdexcept>
#include <string_view>
#include <tuple>

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
constexpr auto log_level_to_string(log_level level) -> std::string
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
		ssm<<std::format("{: <12} ", format("[{}]", log_level_to_string(get_level())));
		
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

class base_logger
{
public:
	base_logger():
		m_msg_que 	{},
		m_running 	{ true },
		m_mtx 		{},
		m_cond 		{},
		m_thd 		{ &base_logger::thd_call_back, this }
	{
		
		//log(log_level::info, "=== logger<{:p}> start ===", reinterpret_cast<void*>(this));
	}

	virtual ~base_logger()
	{
		//log(log_level::info, "=== logger<{:p}> destoried ===", reinterpret_cast<void*>(this));
		m_running.store(false, std::memory_order_release);
		if (m_thd.joinable())
			m_thd.join();
	}

	static auto instance() -> base_logger&
	{
		static base_logger instance;

		return instance;
	}

	void push_msg(std::unique_ptr<base_log_msg> msg)
	{
		std::lock_guard lock { m_mtx };
		m_msg_que.push(std::move(msg));
		m_cond.notify_one();
	}

	void check_fatal(log_level level)
	{
		if (level == log_level::fatal)
		{
			m_running.store(false, std::memory_order_release);
			m_thd.join();
			throw log_fatal_error{};
		}
	}

private:
	void thd_call_back()
	{
		while(m_running.load(std::memory_order_acquire))
		{
			std::unique_lock lock { m_mtx };
			m_cond.wait(lock, [this] {
				return !m_msg_que.empty() || !m_running.load(std::memory_order_acquire);
			});
			
			if (m_running.load(std::memory_order_relaxed)
			 && m_msg_que.empty())
				break;
			
			auto msg = std::move(m_msg_que.front());
			m_msg_que.pop();
			lock.unlock();

			std::ostream* os = nullptr;
			if (msg->get_os())
			{
				os = &msg->get_os()->get();
				(msg->get_os()->get())<<msg->log_string();
			}

			else if(msg->get_level() == log_level::error
				|| msg->get_level() == log_level::fatal)
				os = &std::cerr;
			else
				os = &std::cout;

			(*os)<<msg->log_string();
			if (msg->enable_flush())
				(*os)<<std::endl;
			else
				(*os)<<'\n';
		}
	}
	
private:
	std::queue<std::unique_ptr<base_log_msg>> m_msg_que;
	std::atomic<bool> m_running;
	std::mutex m_mtx;
	std::condition_variable m_cond;
	std::thread m_thd;
};


template<log_level level>
class logger
{
public:
	logger(bool enable_time = false, bool enable_flush = false,
		   std::optional<std::reference_wrapper<std::ostream>> os = std::nullopt):
		m_enable_time { enable_time },
		m_os { os },
		m_enable_flush { enable_flush }
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
	void operator()(std::source_location&& location, std::format_string<Args...> fmt, Args&& ... args)
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
		std::unique_ptr<base_log_msg> p_msg {
			new log_msg<Args...>(level, m_enable_flush, m_os,
					get_time(), location, fmt, std::forward<Args>(args)...)
		};
		base_logger::instance().push_msg(std::move(p_msg));
		base_logger::instance().check_fatal(level);
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
	std::optional<std::reference_wrapper<std::ostream>> m_os;
};

template<log_level level>
struct _simple_log_invoker
{
	template<typename... Args>
	void operator()(std::format_string<Args...> fmt, Args&& ... args)
	{
		log(fmt, std::forward<Args>(args)...);
	}
	
	logger<level> log;
};

template<log_level level>
struct _simple_log_invoker_loc
{
	template<typename... Args>
	void operator()(std::format_string<Args...> fmt, Args&& ... args)
	{
		log(std::source_location::current(), fmt, std::forward<Args>(args)...);
	}
	
	logger<level> log;
};

template<typename... Args>
void info(std::format_string<Args...> fmt, Args&& ... args)
{
	_simple_log_invoker<log_level::info>{}(fmt, std::forward<Args>(args)...);
}

template<typename... Args>
void info_loc(std::format_string<Args...> fmt, Args&& ... args)
{
	_simple_log_invoker_loc<log_level::info>{}(fmt, std::forward<Args>(args)...);
}

}	//namespace yq

#endif	//__EASYLOG_HPP__

