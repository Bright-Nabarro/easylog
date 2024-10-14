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
	warning,
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
	case log_level::warning:
		return "Warning";
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
	base_log_msg(log_level level):
		m_level { level }
	{ }
	virtual ~base_log_msg() = default;
	virtual auto log_string() const -> std::string = 0;
	virtual auto get_level() const -> log_level
	{
		return m_level;
	}
private:
	log_level m_level;
};

template<typename... Args>
class log_msg: public base_log_msg
{	
public:
	log_msg(log_level level, std::optional<std::chrono::system_clock::time_point> time,
			std::optional<std::source_location> location, std::format_string<Args...> fmt,
			Args&& ... args):
		base_log_msg { level },
		m_time { time },
		m_location { location },
		m_fmt { fmt },
		m_args { std::make_tuple(args...) }
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
	const std::tuple<Args...> m_args;
};

class logger final
{
	friend auto logger_instance() -> logger&;
public:
	void set_output_time(bool enable)
	{
		m_enable_time = enable;
	}

	void set_output_stream(std::ostream& os)
	{
		m_os = os;
	}

	template<typename... Args>
	void log(log_level level, std::format_string<Args...> fmt, Args&& ... args)
	{
		// std::format_string may unable forward through make_unique
		std::unique_ptr<base_log_msg> p_msg {
			new log_msg<Args...>(level, get_time(), std::nullopt, fmt, std::forward<Args>(args)...)
		};
		push_msg(std::move(p_msg));
		check_fatal(level);
	}

	template<typename... Args>
	void log(log_level level, std::source_location&& location, std::format_string<Args...> fmt, Args&& ... args)
	{
		// std::format_string may unable forward through make_unique
		std::unique_ptr<base_log_msg> p_msg {
			new log_msg<Args...>(level, get_time(), std::move(location), fmt, std::forward<Args>(args)...)
		};
		push_msg(std::move(p_msg));
		check_fatal(level);

	}

	static auto instance() -> logger&
	{
		static logger instance;

		return instance;
	}

private:
	logger():
		m_enable_time { false },
		m_os 		{ std::ref(std::cout) },
		m_msg_que 	{},
		m_running 	{ true },
		m_mtx 		{},
		m_cond 		{},
		m_thd 		{ &logger::thd_call_back, this }
	{
		if (!m_os)
			throw log_construct_error { std::format("Logger stream error") };

		log(log_level::info, "=== logger<{:p}> start ===", reinterpret_cast<void*>(this));
	}

	~logger()
	{
		log(log_level::info, "=== logger<{:p}> destoried ===", reinterpret_cast<void*>(this));
		m_running.store(false, std::memory_order_release);
		if (m_thd.joinable())
			m_thd.join();
	}

	auto get_time() const -> std::optional<std::chrono::system_clock::time_point>
	{
		if (m_enable_time)
		{
			return std::chrono::system_clock::now();
		}
		return std::nullopt;
	}
	
	void push_msg(std::unique_ptr<base_log_msg> msg)
	{
		std::lock_guard lock { m_mtx };
		m_msg_que.push(std::ref(msg));
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

	void thd_call_back()
	{
		while(m_running.load(std::memory_order_acquire))
		{
			std::unique_lock lock { m_mtx };
			m_cond.wait(lock, [this] {
				return !m_msg_que.empty() || !m_running.load(std::memory_order_acquire);
			});
			
			if (m_running.load(std::memory_order_relaxed)
			 || m_msg_que.empty())
				break;
			
			auto msg = std::move(m_msg_que.front());
			m_msg_que.pop();
			lock.unlock();

			if (m_os)
				(m_os->get())<<msg->log_string()<<'\n';
			else if(msg->get_level() == log_level::error
				|| msg->get_level() == log_level::fatal)
				std::cerr<<msg->log_string()<<'\n';	
			else
				std::cout<<msg->log_string()<<'\n';
		}
	}

private:
	bool m_enable_time;
	std::optional<std::reference_wrapper<std::ostream>> m_os;
	std::queue<std::unique_ptr<base_log_msg>> m_msg_que;
	std::atomic<bool> m_running;
	std::mutex m_mtx;
	std::condition_variable m_cond;
	std::thread m_thd;
};



}	//namespace yq

#endif	//__EASYLOG_HPP__
		
